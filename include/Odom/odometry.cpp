#include "odometry.h"
#include <math.h>
#include <iostream>


#define M_PI 3.14159265358979323846  /* pi */
#define WHEEL_RADIUS 0.15
#define DIST_BETWEEN_WHEELS 0.5

nav_msgs::Odometry OdometryCalculator::getOdomMessage() {
	return _odom;
}

geometry_msgs::TransformStamped OdometryCalculator::getTfMessage() {
	return _odom_tf;
}

OdometryCalculator::OdometryCalculator(int32_t encoder_resolution) {
	_encoder_resolution = encoder_resolution;

	_x = 0;
	_v_x = 0;
	_y = 0;
	_v_y = 0;
	_theta = 0;
	_v_theta = 0;

	_impulses_1 = 0;
	_impulses_2 = 0;
}

void OdometryCalculator::updateOdometry(int32_t new_impulses_1, int32_t new_impulses_2, ros::Time new_time_1, ros::Time new_time_2) {
	//Convert impulses to distance travelled by wheel
	double d_1 = ((new_impulses_1 - _impulses_1) / 4096.0f) * 2 * M_PI * WHEEL_RADIUS;
	double d_2 = ((new_impulses_2 - _impulses_2) / 4096.0f) * 2 * M_PI * WHEEL_RADIUS;

	double total_d = (d_1 + d_2) / 2;
	double theta_diff = (d_1 - d_2) / DIST_BETWEEN_WHEELS;

	double x_diff = total_d * cos( _theta + (theta_diff / 2.0f));
	double y_diff = total_d * sin( _theta + (theta_diff / 2.0f));

	double dt = (new_time_1 - _read_time_1).toSec();
	_v_x = x_diff / dt;
	_v_x = y_diff / dt;
	_v_theta = theta_diff / dt;

	_x = _x + x_diff;
	_y = _y + y_diff;
	_theta = _theta + theta_diff;

	std::cout << "Position: X = " << _x << " Y = " << _y << " Theta = " << _theta << std::endl;

	//Update saved variables
	_impulses_1 = new_impulses_1;
	_impulses_2 = new_impulses_2;
	_read_time_1 = new_time_1;
	_read_time_2 = new_time_2;
}