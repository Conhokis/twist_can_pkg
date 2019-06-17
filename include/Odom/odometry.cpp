#include "odometry.h"
#include <math.h>
#include <iostream>


#define M_PI 3.14159265358979323846  /* pi */
#define WHEEL_RADIUS 0.13
#define DIST_BETWEEN_WHEELS 0.446

nav_msgs::Odometry OdometryCalculator::getOdomMessage() {
	return _odom;
}

geometry_msgs::TransformStamped OdometryCalculator::getTfMessage() {
	return _odom_tf;
}

OdometryCalculator::OdometryCalculator(int32_t encoder_resolution, int32_t impulses_1, int32_t impulses_2) {
	_encoder_resolution = encoder_resolution;

	_x = 0;
	_v_x = 0;
	_y = 0;
	_v_y = 0;
	_theta = 0;
	_v_theta = 0;

	_impulses_1 = impulses_1;
	_impulses_2 = impulses_2;
	double d_1 = ((_impulses_1 / 4096.0f) / 16) * 2 * M_PI * WHEEL_RADIUS;
	double d_2 = ((_impulses_2 / 4096.0f) / 16) * 2 * M_PI * WHEEL_RADIUS;
}

void OdometryCalculator::updateOdometry(int32_t new_impulses_1, int32_t new_impulses_2, ros::Time current_time) {
	//Convert impulses to distance travelled by wheel
	double d_1 = (((new_impulses_1 - _impulses_1) / 4096.0f) / 16) * 2 * M_PI * WHEEL_RADIUS;
	double d_2 = (((new_impulses_2 - _impulses_2) / 4096.0f) / 16) * 2 * M_PI * WHEEL_RADIUS;

	double total_d = (d_1 + d_2) / 2;
	double theta_diff = ((d_1 - d_2) / DIST_BETWEEN_WHEELS);

	double x_diff = total_d * cos( _theta + (theta_diff / 2.0f));
	double y_diff = total_d * sin( _theta + (theta_diff / 2.0f));

	double dt = (current_time - _last_time).toSec();
	_v_x = x_diff / dt;
	_v_y = y_diff / dt;
	_v_theta = theta_diff / dt;

	_x = _x + x_diff;
	_y = _y + y_diff;
	_theta = _theta + theta_diff;

	//std::cout << "Position: X = " << _x << " Y = " << _y << " Theta = " << _theta << std::endl;
	geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(_theta);

	//Header of tf
	_odom_tf.header.stamp = current_time;
	_odom_tf.header.frame_id = "odom";
	_odom_tf.child_frame_id = "base_link";

	//Pose of tf
	_odom_tf.transform.translation.x = _x;
	_odom_tf.transform.translation.y = _y;
	_odom_tf.transform.translation.z = 0.0;
	_odom_tf.transform.rotation = odom_quat;

	//Pose or nav_msg/odometry
	//Header
	_odom.header.stamp = current_time;
	_odom.header.frame_id = "odom";

	//Pose
	_odom.pose.pose.position.x = _x;
	_odom.pose.pose.position.y = _y;
	_odom.pose.pose.position.z = 0.0;
	_odom.pose.pose.orientation = odom_quat;

	//Set velocity
	_odom.child_frame_id = "base_link";
	_odom.twist.twist.linear.x = _v_x;
	_odom.twist.twist.linear.y = _v_y;
	_odom.twist.twist.angular.z = _v_theta;

	//Update saved variables
	_impulses_1 = new_impulses_1;
	_impulses_2 = new_impulses_2;
	_last_time = current_time;
}
