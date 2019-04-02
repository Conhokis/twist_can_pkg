//ROS include for twist
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <ros/time.h>
#include <iostream>

//For modifying shutdown
#include <ros/xmlrpc_manager.h>
#include <signal.h>

//Include for can interface
#include <can_utils/canUtils.h>

//Time in ns for the topic /cmd_vel, currently 100 ms.
ros::Time start_time;

//FOR NOW DUE TO TELEOP KEYBOARD KEYS, torna o input num degrau
#define MIN_HITS_KEY 5
unsigned int count = 0;

// Signal-safe flag for whether shutdown is requested
sig_atomic_t volatile g_request_shutdown = 0;

void mySigIntHandler(int sig)
{
    g_request_shutdown = 1;
}

void updateTwistSpeed(const geometry_msgs::Twist::ConstPtr& msg) {
    start_time = ros::Time::now();

    if(count < 5) count++;
 
    else std::cout << "Receiving messages" << std::endl;
}

void shutdownCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    std::cout << "teste\n" << std::endl;
}

int main(int argc, char **argv) {
    //Connection information
    const char* interface_id = "can0";
    const unsigned int node_id1 = 0x11;
    const unsigned int node_id2 = 0x12;

    //Start node interface with connection name and controller node-id
    //canMotorInterface canMI(interface_id, node_id1);

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

    ros::Duration TWIST_TIMEOUT = ros::Duration(0.1);
    start_time = ros::Time::now() - TWIST_TIMEOUT;    
    
    //Fazer um write e ler com o candump
    //canMI.canTestWrite();
    //canMI.canRead();
    
    while (ros::ok() && !g_request_shutdown) {
        //Check if master is running
        ros::spinOnce();

        if((ros::Time::now() - start_time) > TWIST_TIMEOUT) {
            std::cout << "No messages received" << std::endl;
            count = 0;
        }

        loop_rate.sleep();
    }

    ros::shutdown();
}
