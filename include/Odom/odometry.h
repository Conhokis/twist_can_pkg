#include <ros/time.h>
#include <stdint.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <iostream>


class OdometryCalculator {
private:
	//Encoder resolution
	int32_t _encoder_resolution;
	//Position values
	int32_t _x, _y, _theta;
	//Encoder impulse counters
	int32_t _impulses_1, _impulses_2;
	//Times of read;
	ros::Time _read_time_1, _read_time_2;
	//ROS message for tf broadcast
	geometry_msgs::TransformStamped _odom_tf;
	//ROS message for publishing odometry
	nav_msgs::Odometry _odom;

public:
	OdometryCalculator(int32_t encoder_resolution);

	//Reads encoders and updates speed, positions and the desired messages
	void updateOdometry(int32_t new_impulses_1, int32_t new_impulses_2, ros::Time new_time_1, ros::Time new_time_2);
	//Returns the odometry message to be published
	nav_msgs::Odometry getOdomMessage();
	//Returns the tf broadcast to be published
	geometry_msgs::TransformStamped getTfMessage();
};