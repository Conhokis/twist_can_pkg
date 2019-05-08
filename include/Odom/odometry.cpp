#include "odometry.h"

nav_msgs::Odometry OdometryCalculator::getOdomMessage() {
	return _odom;
}

geometry_msgs::TransformStamped OdometryCalculator::getTfMessage() {
	return _odom_tf;
}

OdometryCalculator::OdometryCalculator(int32_t encoder_resolution) {
	_encoder_resolution = encoder_resolution;

	_x = 0;
	_y = 0;
	_theta = 0;

	_impulses_1 = 0;
	_impulses_2 = 0;
}

void OdometryCalculator::updateOdometry(int32_t new_impulses_1, int32_t new_impulses_2, ros::Time new_time_1, ros::Time new_time_2) {
	std::cout << "Motor turn 2: " << ((new_impulses_2 / _encoder_resolution)) << std::endl; 
	std::cout << "Motor turn 1: " << ((new_impulses_1 / _encoder_resolution)) << std::endl;
}