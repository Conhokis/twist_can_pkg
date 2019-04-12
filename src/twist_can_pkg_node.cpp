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
canMotorInterface canMI(canBH, node_id[0]);

// Signal-safe flag for whether shutdown is requested
sig_atomic_t volatile g_request_shutdown = 0;

//Catches ctrl-c events, this is tested and working
void mySigIntHandler(int sig)
{
    canMI.shutdownMotor();
    exit(0);
}

//Catches roskill events
void shutdownCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    canMI.shutdownMotor();
    exit(0);
}

//=============================================================
//Time in ns for the topic /cmd_vel, currently 100 ms.
ros::Time start_time;

//FOR NOW DUE TO TELEOP KEYBOARD KEYS, torna o input num degrau
#define MIN_HITS_KEY 5
unsigned int count = 0;
int16_t lin_vel = 0;

void updateTwistSpeed(const geometry_msgs::Twist::ConstPtr& msg) {
    start_time = ros::Time::now();

    if(count < 5) count++;
 
    else {
        //lin_vel = msg->linear.x * 400;
        std::cout << "Received " << msg->linear.x << std::endl;
    }
}

int main(int argc, char **argv) {
    //ROS CONFIGURTIONS
    //Configurations of the node
    ros::init(argc, argv, "example_node", ros::init_options::NoSigintHandler);
    ros::NodeHandle n("~");
    ros::Rate loop_rate(100);

    //Configuration for catching shutdown event
    signal(SIGINT, mySigIntHandler);
    ros::XMLRPCManager::instance()->unbind("shutdown");
    ros::XMLRPCManager::instance()->bind("shutdown", shutdownCallback);

    //Subscribe to /cmd_vel
    ros::Subscriber sub = n.subscribe("/cmd_vel", 10, updateTwistSpeed);

    //To check if theres is input or not from twist_cmd_vel
    ros::Duration TWIST_TIMEOUT = ros::Duration(0.1);
    start_time = ros::Time::now() - TWIST_TIMEOUT;    

    //Sends a message to the motor and waits for the receive
    canMI.checkMotorStatus();

    //Power on motor
    canMI.powerOnMotor();

    //
    
    while (ros::ok() && !g_request_shutdown) {
        //Check if master is running
        ros::spinOnce();

        //If no messages received, enters this if
        if((ros::Time::now() - start_time) > TWIST_TIMEOUT) {
            std::cout << "No messages received" << std::endl;
            //canMI.setTargetVelocity(0);
            count = 0;
        }

        else {
            //canMI.setTargetVelocity(lin_vel);
        }

        loop_rate.sleep();
    }

    ros::shutdown();
}