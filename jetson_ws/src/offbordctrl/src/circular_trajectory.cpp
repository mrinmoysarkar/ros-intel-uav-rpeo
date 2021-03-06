// author: Mrinmoy Sarkar
// email: mrinmoy.pol@gmail.com
// date: 10/03/2018

#include <cstdlib>
#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <sensor_msgs/Imu.h>
#include <mavros_msgs/Altitude.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Pose.h>
#include <tf/transform_datatypes.h>
#include <std_msgs/String.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <cmath>

using namespace std;


//Global Variable
double xPose;
double yPose;
double zPose;
double roll_angle;
double pitch_angle;
double yaw_angle;

double del = 0.1;
double del_time = 3.0;
int indx = 0;

int maxidx = 0;
const int max_point=10000;
double x[max_point];
double y[max_point];
double z[max_point];

void create_data()
{
    double xc=-1,yc=0,r=1;
    int idx=0;
    double alt = 2.0;
    double dx = 0.1;
    for(double i=0;i>=-r*2;)
    {
        x[idx] = i;
        y[idx] = yc+sqrt(r*r-(i-xc)*(i-xc));
        z[idx] = alt;
        i-=dx;
        idx++;
    }
    for(double i=-2*r;i<=0;)
    {
        x[idx] = i;
        y[idx] = yc-sqrt(r*r-(i-xc)*(i-xc));
        z[idx] = alt;
        i+=dx;
        idx++;
    }
    maxidx=idx;
}

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}


void pose_cb(const geometry_msgs::PoseStamped::ConstPtr & msg){
    xPose = msg->pose.position.x;
    yPose = msg->pose.position.y;
    zPose = msg->pose.position.z;
    double quatx= msg->pose.orientation.x;
    double quaty= msg->pose.orientation.y;
    double quatz= msg->pose.orientation.z;
    double quatw= msg->pose.orientation.w;

    tf::Quaternion q(quatx, quaty, quatz, quatw);
    tf::Matrix3x3 m(q);
    m.getRPY(roll_angle, pitch_angle, yaw_angle);
    //cout << quatx << " " << quaty << " " << " " << quatz << " " << quatw << endl;
    if(abs(xPose-x[indx])<del && abs(yPose-y[indx])<del)
    {
        //indx++;
        //if(indx==5)indx=1;
    }
}


int main(int argc, char **argv)
{
    //char buff[100];
    //getcwd(buff,100);
    //cout << buff <<endl;
    create_data();
    ros::init(argc, argv, "circular_trajectory_node");
    ros::NodeHandle nh;

    //Subscribers
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);

    ros::Subscriber position_sub = nh.subscribe("mavros/local_position/pose", 100, pose_cb);

    //Publisher
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 100);

    //Service Clients
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("mavros/cmd/arming");

    ros::ServiceClient land_client = nh.serviceClient<mavros_msgs::CommandTOL>
      ("mavros/cmd/land");

    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("mavros/set_mode");



  
   
    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);
    while(ros::ok() && current_state.connected){
        ros::spinOnce();
        rate.sleep();
        ROS_INFO("connecting to FCT...");
    }

    geometry_msgs::PoseStamped pose;

    pose.pose.position.x = 0;
    pose.pose.position.y = 0;
    pose.pose.position.z = 0;

    //send a few setpoints before starting
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }

    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    mavros_msgs::CommandTOL land_cmd;
    land_cmd.request.yaw = 0;
    land_cmd.request.latitude = 0;
    land_cmd.request.longitude = 0;
    land_cmd.request.altitude = 0;

    ros::Time last_request = ros::Time::now();

    // change to offboard mode and arm
    while(ros::ok() && !current_state.armed){
        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
          ROS_INFO(current_state.mode.c_str());
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {

            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }
    
	
    int j = 1;
    double previous_time = ros::Time::now().toSec();
    double first_prev_time = previous_time;
    while(ros::ok())
    {
        double current_time = ros::Time::now().toSec();
        pose.pose.position.x = x[indx];
        pose.pose.position.y = y[indx];
        pose.pose.position.z = z[indx];

        if((current_time -first_prev_time)> 60)
        {
            break;
        }
        //Publishing position
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
        if((current_time -previous_time)> .3)
        {
           previous_time = current_time;
           indx++;
        }
        if(indx==maxidx)indx=0;
    } // end while loop

    //Start landing command
    ROS_INFO("tring to land");
    while (!(land_client.call(land_cmd) &&
            land_cmd.response.success)){
      ROS_INFO("tring to land");
      ros::spinOnce();
      rate.sleep();
    }
    return 0;
}
