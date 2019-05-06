//ROS include for twist
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <ros/time.h>
#include <iostream>
#include <stdint.h>

//For modifying shutdown
#include <ros/xmlrpc_manager.h>
#include <signal.h>

//Include for can interface
#include <can_utils/canUtils.h>

//Connection information
const char* interface_id = "can0";
unsigned int node_id[] = {0x11, 0x12};
size_t size_node_id = sizeof(node_id);
//Start canBusHandler, manages connection and executes read and write functions
canBusHandler* canBH = new canBusHandler(interface_id, node_id, size_node_id);  
//Start node interface with connection name and controller node-id
canMotorInterface canMI_1(canBH, node_id[0]);
canMotorInterface canMI_2(canBH, node_id[1]);

// Signal-safe flag for whether shutdown is requested
sig_atomic_t volatile g_request_shutdown = 0;

//Catches ctrl-c events, this is tested and working
void mySigIntHandler(int sig)
{
    g_request_shutdown = 1;
}

//Catches roskill events
void shutdownCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    g_request_shutdown = 1;
}

//=============================================================
//Time in ns for the topic /cmd_vel, currently 100 ms.
ros::Time start_time;

//FOR NOW DUE TO TELEOP KEYBOARD KEYS, torna o input num degrau
#define MIN_HITS_KEY 4
unsigned int count = 0;
int16_t lin_vel = 0;

void updateTwistSpeed(const geometry_msgs::Twist::ConstPtr& msg) {
    start_time = ros::Time::now();

    if(count < MIN_HITS_KEY) count++;
 
    else {
        lin_vel = msg->linear.x * 600;
    }
}

int main(int argc, char **argv) {
    //ROS CONFIGURTIONS
    //Configurations of the node
    ros::init(argc, argv, "example_node", ros::init_options::NoSigintHandler);
    ros::NodeHandle n("~");
    ros::Rate loop_rate(50);

    //Configuration for catching shutdown event
    signal(SIGINT, mySigIntHandler);
    ros::XMLRPCManager::instance()->unbind("shutdown");
    ros::XMLRPCManager::instance()->bind("shutdown", shutdownCallback);

    //Subscribe to /cmd_vel
    ros::Subscriber sub = n.subscribe("/cmd_vel", 10, updateTwistSpeed);

    //To check if theres is input or not from twist_cmd_vel
    ros::Duration TWIST_TIMEOUT = ros::Duration(0.05);
    start_time = ros::Time::now() - TWIST_TIMEOUT;    

    //Sends a message to the motor and waits for the receive, check if motor is connected
    canMI_1.checkMotorStatus();
    canMI_2.checkMotorStatus();

    //Power on motor
    canMI_1.powerOnMotor();
    canMI_2.powerOnMotor();
    
    ros::Time start_loop_time;
    ros::Time end_loop_time;
    while (ros::ok() && !g_request_shutdown) {
        //Check if master is running
        ros::spinOnce();
        start_loop_time = ros::Time::now();

        //If no messages received, enters this if
        if((ros::Time::now() - start_time) > TWIST_TIMEOUT) {
            lin_vel = 0;
            count = 0;
        }

        canMI_1.setTargetVelocity(lin_vel);
        canMI_2.setTargetVelocity(lin_vel);

        end_loop_time = ros::Time::now();
        //Uncomment for loop times
        //std::cout << end_loop_time - start_loop_time << std::endl;
        std::cout << "Motor 1: " << canMI_1.readMotorSpeed() << std::endl;
        std::cout << "Motor 2: " << canMI_2.readMotorSpeed() << std::endl;
        loop_rate.sleep();
    }

    //Shutdown tasks, in this case power off motors and engage brake
    canMI_1.shutdownMotor();
    canMI_2.shutdownMotor();

    ros::shutdown();
}