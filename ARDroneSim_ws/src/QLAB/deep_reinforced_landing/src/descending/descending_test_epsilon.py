#!/usr/bin/env python

# The MIT License (MIT)
# Copyright (c) 2017 Riccardo Polvara
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
# FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE # SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
# DQN tensorflow implementation for achieving autonomous landing.
import random as STDrandom
import tensorflow as tf
import numpy as np
import sys
from cv_bridge import CvBridge, CvBridgeError
from q_network import QNetwork
from experience_replay_buffer import ExperienceReplayBuffer
import Image
import datetime
import time
import os.path
import cv2
# Adding these two lines solved the crash of Tesla K40
import os
os.environ["CUDA_VISIBLE_DEVICES"] = "0"
from deep_reinforced_landing.srv import NewCameraService, GetDoneAndReward, SendCommand, ResetPosition, GetRelativePose # DRL services
from gazebo_msgs.srv import DeleteModel # Gazebo service for removing a model
import rospy
# Rename to avoid confusion with Image lib
from sensor_msgs.msg import Image as ROSImage
import subprocess # needed for using bash command


DEBUG = False  # Set to False to disable the image shown at the begining
_last_image = None  # Global variable representing the last frame acquired by the camera
#_save_image = False


def image_callback(img_msg):
    """
    When a new image is published, save the last frame in a global variable
    """
    bridge = CvBridge()
    try:
        # Convert from sensor_msgs::Image to cv::Mat
        cv_image = bridge.imgmsg_to_cv2(
            img_msg, desired_encoding="passthrough")
        # Access global variable and store image as numpy.array
        global _last_image
        _last_image = np.asarray(cv_image)
    except CvBridgeError as ex:
        print "ERROR!!"
        print ex



def get_image():
    """
    Get the last frame acquired by the camera.

    @return resp.image is the gresycale image acquired by the camera
    """

    # Access the global variable and activate the saving for the last camera's
    # frame
    global _save_image
    _save_image = True


def get_done_reward():
    """
    Get the done status and the reward after completing an action.

    @return resp contains the reward and the done status
    """
    rospy.wait_for_service('/drl/get_done_reward')

    try:
        get_done_reward_proxy = rospy.ServiceProxy(
            '/drl/get_done_reward', GetDoneAndReward)
        resp = get_done_reward_proxy()
        return resp
    except rospy.ServiceException, ex:
        print "Service call get_reward_done failed: %e" % ex

def remove_model(model):
    """
    Remove the model from the world. 
    @param model is the name of the model to remove
    """
    rospy.wait_for_service('/gazebo/delete_model')
    try:
        remove_model_proxy = rospy.ServiceProxy('/gazebo/delete_model', DeleteModel)
        remove_model_proxy(model)
    except rospy.ServiceException, ex:
        print "Service call delete_model failed: %e" % ex

def clean_world(ground_list):
    for ground in ground_list:
        try:
            ground = ground + "_plane"
            remove_model(ground)
        except:
            pass


def generate_new_world(model_to_add, ground_list):
    """
    Remove the old model on the floor and add new ones.

    @param model_to_remove is the name of the model to remove (all its istances)
    @param model_to_add is the name of the model to add
    """
    
    if type(model_to_add) is int:
        #print "Is an int!"
        model_to_add = ground_list[model_to_add] 
    
    print "\n"
    rospy.logwarn( "Ground choosen is " + str(model_to_add) )   
    # Spawn new istances for the new model
    os.system("rosrun gazebo_ros spawn_model -file ~/.gazebo/models/" + model_to_add +"/model.sdf -sdf -model " + model_to_add + "_plane -x 0 -y 0")


def get_random_action():
    """
    Choose a random action for the UAV.
    """
    # Define an array containing the available actions for the UAV
    # in the final work, takeoff and land must be added
    action_list = ['left', 'right', 'forward', 'backward', 'stop', 'descend']
    # Choose a random action within the array
    #action_index = STDrandom.randint(0, len(action_list) - 1)
    # forward,backward,left,right, stop and land
    probability_descend = 0.25
    probability = (1.0 - probability_descend)/ (len(action_list) -1)
    action_probability = [probability, probability, probability, probability, probability, probability_descend]
    action = np.random.choice(action_list, 1, p=action_probability)[0]
    #action_index = STDrandom.randint(0, 10)
    #action = action_list[action_index]

    return action


def send_action(action):
    """
    Send an action to the UAV.
    """
    rospy.wait_for_service('/drl/send_command')
    try:
        get_random_action_proxy = rospy.ServiceProxy('/drl/send_command', SendCommand)
        # Call the service
        get_random_action_proxy(action)
    except rospy.ServiceException, ex:
        print "Service call get_random_action failed: %s" % ex


def reset_pose():
    """
    Reset the UAV's randomly inside the flight Bounding Box.
    """
    rospy.wait_for_service('/drl/set_model_state')
    try:
        reset_pose_proxy = rospy.ServiceProxy('/drl/set_model_state', ResetPosition)
        reset_pose_proxy(True)
    except rospy.ServiceException, ex:
        print "Service call reset_pose failed: %s" % ex


def convert_action_batch_str_to_int(action_t_batch):
    """
    Convert a batch containing actions expressed as string in integer
    (as required by the gradient descent method).

    @param action_batch is the array contatining actions as string
    @return action_batch is the array containing actions as integer
    """

    #action_list = ['left', 'right', 'forward', 'backward', 'stop', 'land', 'left_forward', 'left_backward', 'right_forward', 'right_backward' ,'descend', 'ascend', 'rotate_left', 'rotate_right']
    for i in range(len(action_t_batch)):
        if action_t_batch[i] == 'left':
            action_t_batch[i] = 0
        elif action_t_batch[i] == 'right':
            action_t_batch[i] = 1
        elif action_t_batch[i] == 'forward':
            action_t_batch[i] = 2
        elif action_t_batch[i] == 'backward':
            action_t_batch[i] = 3
        elif action_t_batch[i] == 'stop':
            action_t_batch[i] = 4
        elif action_t_batch[i] == 'descend':
            action_t_batch[i] = 5
    return action_t_batch


def convert_action_int_to_str(action):
    """
    Convert an action expressed as integer into its string value.

    @param action is an integer representing which action UAV has to take
    @return action as string value
    """
    #action_list = ['left', 'right', 'forward', 'backward', 'stop', 'land', 'left_forward', 'left_backward', 'right_forward', 'right_backward' ,'descend', 'ascend', 'rotate_left', 'rotate_right']
    # Conversion from integer to string
    if action == 0:
        action = 'left'
    elif action == 1:
        action = 'right'
    elif action == 2:
        action = 'forward'
    elif action == 3:
        action = 'backward'
    elif action == 4:
        action = 'stop'
    elif action == 5:
        action = 'descend'
    return action

def get_relative_pose(ground_index, episode, step, reward):
    """
    Get the relative pose of the quadrotor to the marker.
    @return a log containing information such as the ground used, episode, step, reward and the relative pose of the quadrotor
    """
    rospy.wait_for_service('/drl/get_relative_pose')
    try:
        get_relative_pose_proxy = rospy.ServiceProxy(
            '/drl/get_relative_pose', GetRelativePose)
        resp = get_relative_pose_proxy()

        with open("./relative_distances_" + str(datetime.date.today().strftime("%m%d%Y")) + ".csv", "a") as myfile:
            ground_list = ["asphalt11", "asphalt12", "asphalt13",
                           "brick11", "brick12", "brick13", 
                           "grass11", "grass12", "grass13", 
                           "pavement11", "pavement12", "pavement13", 
                           "sand11", "sand12", "sand13", 
                           "snow11", "snow12", "snow13", 
                           "soil11", "soil12", "soil13"]

            string_to_add = ground_list[ground_index] + "," + str(episode) + "," + str(step) + "," + str(reward) + "," + str(resp.pose.position.x) + "," + str(resp.pose.position.y) + "," + str(resp.pose.position.z) + '\n'
            myfile.write(string_to_add)
    except rospy.ServiceException, ex:
        print "Service call get_relative_pose failed: %e" % ex


def main():
    """
    Main function for training the DQN network in learning how to accomplish autonomous landing.
    """
    # ATTENTION: If you want to restore files from a previous simulation you
    # must pass valid values for these variables:
    policy_weights_path = '/home/pulver/Desktop/simulation_8/checkpoint/episode_20500/policy/policy_checkpoint.ckp'
    summary_folder = ""  # if empty the summary is written in ./log/ + current time
    start_episode = 0  # change to last episode number
    frame_counter = 0  # change to last number of frames
    epsilon = 0.1
    screen_width = 84  # original is 160
    screen_height = 84  # original is 210
    images_stack_size = 4
    # Use only the first 5 actions for this simulation
    # action_list = ['left', 'right', 'forward', 'backward', 'stop', 'descend']
    # 0 (left, 1 (right), 2 (forward), 3 (backward), 4 (stop), 5 (descend)
    tot_actions = 6
    batch_size = 64  # size of the experience batch
    tot_steps = 1000  # finite-horizont simulation
    # 10x10^6 high number since training can be stopped at every time
    tot_episodes = 210
    steps_per_episodes = 40  # expressed in number step
    noop_time = 2.0  # pause in seconds between actions

    num_ground_plane = 70
    actual_ground_index = 0
    # episode_per_ground specify the number of episodes with the same ground plane
    ground_counter = 1
    episode_per_ground = 10

    timer_total_start = time.time()
    rospy.init_node("DRLTestNode")
    rospy.loginfo("----- DRL Test Node -----")

    # Create a subscriber fot the greyscale image
    # rospy.Subscriber("/drl/grey_camera", ROSImage, image_callback)#TODO
    # restore default
    rospy.Subscriber(
        "/quadrotor/ardrone/bottom/ardrone/bottom/image_raw", ROSImage, image_callback,queue_size=30)  # Store the last 30 messages before discarding them

    images_stack_size = 4

    r = rospy.Rate(10)  # 10hz
    ground_list = ["asphalt11", "asphalt12", "asphalt13",
                   "brick11", "brick12", "brick13",
                   "grass11", "grass12", "grass13",
                   "pavement11", "pavement12", "pavement13",
                   "sand11", "sand12", "sand13",
                   "snow11", "snow12", "snow13",
                   "soil11", "soil12", "soil13"]

    # Init session and networks
    sess = tf.Session()
    if(summary_folder == ""):
        tf_summary_writer = tf.summary.FileWriter(
            './log/' + str(datetime.datetime.now().time()), sess.graph)
    else:
        tf_summary_writer = tf.summary.FileWriter(
            summary_folder, sess.graph)
    policy_network = QNetwork(sess, tot_actions=tot_actions, image_shape=(
        screen_width, screen_height, images_stack_size), batch_size=batch_size, network_name="policy_net")
    
    init = tf.global_variables_initializer()
    sess.run(init)

    
    if(policy_weights_path != ""):
        print("Loading weights from memory: " + str(policy_weights_path))
        policy_network.load_weights(policy_weights_path)
    else:
        print("The networks path are empty. Choose appropriate weights!")
        sys.exit()

    # start here
    for episode in range(start_episode, tot_episodes):
        # Reset the ground in the environment every 50 episodes (or episode_per_ground)
        ground_index = episode / episode_per_ground
        if ground_index != actual_ground_index or (episode == 0):
            clean_world(ground_list)
            generate_new_world(ground_index, ground_list)
        actual_ground_index = ground_index

        timer_start = time.time()
        actual_time = rospy.get_rostime()
        rospy_start_time = actual_time.secs + actual_time.nsecs / 1000000000.0
        frame_episode = 0
        cumulated_reward = 0
        epsilon_used = 0
        send_action("takeoff")
        print("")
        print("Episode: " + str(episode))
        # 1-Accumulate the first state
        reset_pose()
        print "Reset pose!"
        send_action("stop")
        rospy.sleep(1.0)
#        get_image()
        state = _last_image
        # create a stack of X images
        image_t = np.stack([state] * images_stack_size, axis=2)

        for step in range(tot_steps):
            # 2- Get the action following epsilon-greedy or through policy network.
            # Here image_t is always ready and it can be given as input to
            # the network
            epsilon_used_bool = False
            if(np.random.random_sample(1) < epsilon):
                epsilon_used += 1
                action = get_random_action()
                epsilon_used_bool = True
            # Take the action from the policy network
            else:
                # Here image_t is always ready and it can be given as input to
                # the network
                action_distribution = policy_network.return_action_distribution(
                    input_data=np.reshape(image_t, (1, 84, 84, images_stack_size)), softmax=False)
                action = np.argmax(action_distribution)
                action = convert_action_int_to_str(action)
            #print action
            send_action(action)
            rospy.sleep(noop_time)
            # 3- Get the state_t1 repeating the action obtained at step-2 and add everything
            # in the replay buffer. Then set state_t = state_t1
            #get_image()
            image_t1 = _last_image
            send_action("stop")
            # If the action taken is to land and the UAV is inside the landing BB, done will be calculated accordingly
            done_reward = get_done_reward()
            reward = done_reward.reward
            done = done_reward.done
            if epsilon_used_bool:
                print " Step(" + str(step) + "), Action: " + action + ", Altitude: " + str(done_reward.z) + ", Reward: " + str(reward)
            else:
                print "#Step(" + str(step) + "), Action: " + action + ", Altitude: " + str(done_reward.z) + ", Reward: " + str(reward)
                # ['left', 'right', 'forward', 'backward', 'stop', 'land'
                print(" left:" + str(action_distribution[0][0][0]) + "; right:" + str(action_distribution[0][0][1]) + "; forward:" + str(action_distribution[0][0][2]) + "; backward:" + str(action_distribution[0][0][3]) + "; stop:" + str(action_distribution[0][0][4])  + "; descend:" + str(action_distribution[0][0][5]))
            image_t1 = np.expand_dims(image_t1, 2)
            # stack the images
            image_t1 = np.append(image_t[:, :, 1:], image_t1, axis=2)
            frame_counter += 1  # To call every time a frame is obtained
            frame_episode += 1
            image_t = image_t1
            get_relative_pose(ground_index, episode, step, reward)
            cumulated_reward += reward
            # At every step check if more than 30 seconds from the start passed.
            # In that case, set done = True and end the episode
            timer_stop = time.time()
            # Stop the episode if the number of frame is more than a threshold
            if frame_episode >= steps_per_episodes:
                done = True
            # When the episode is done
            if done:
                timer_stop = time.time()
                actual_time = rospy.get_rostime()
                rospy_stop_time = actual_time.secs + actual_time.nsecs / 1000000000.0
                rospy_time_elapsed = rospy_stop_time - rospy_start_time
                print("Tot Frame counter: " + str(frame_counter))
                print("Time episode: " + str(timer_stop - timer_start) + " seconds")
                print("Ros time episode: " +
                      str(rospy_time_elapsed) + " seconds")
                if cumulated_reward >= 0:
                    rospy.logwarn("Positive reward obtained!")
                print("Cumulated reward: " + str(cumulated_reward))
                print("Episode finished after {} timesteps".format(step + 1))
                print("Epsilon used: " + str(epsilon_used) + " out of " + str(step + 1) + "(" + str(float((epsilon_used * 100.0) / (step + 1.0))) + "%)")
                sys.stdout.flush()
                time.sleep(5)
                break


if __name__ == "__main__":
    main()
