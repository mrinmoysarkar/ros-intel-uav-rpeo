cd $(pwd)/src/Firmware/

source Tools/setup_gazebo.bash $(pwd) $(pwd)/build/px4_sitl_default
export ROS_PACKAGE_PATH=$ROS_PACKAGE_PATH:$(pwd):$(pwd)/Tools/sitl_gazebo
#roslaunch px4 four_uav_mavros_sitl_custom.launch
roslaunch pie multi_copter_sim.launch



