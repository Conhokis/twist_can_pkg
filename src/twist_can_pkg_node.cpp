//ROS include for twist
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <ros/time.h>
#include <iostream>
#include <signal.h>

//Include for can interface
#include <can_utils/canUtils.h>

//Time in ns for the topic /cmd_vel, currently 100 ms.
ros::Time start_time;

//FOR NOW DUE TO TELEOP KEYBOARD KEYS, torna o input num degrau
#define MIN_HITS_KEY 5
unsigned int count = 0;

void updateTwistSpeed(const geometry_msgs::Twist::ConstPtr& msg) {
    start_time = ros::Time::now();

    if(count < 5) count++;
 
    else std::cout << "Receiving messages" << std::endl;
}

void systemPowerOff(int s) {
    printf("teste\n");
}

int main(int argc, char **argv) {
    //Connection information
    const char* interface_id = "can0";
    const unsigned int node_id1 = 0x11;
    const unsigned int node_id2 = 0x12;

    //Start node interface with connection name and controller node-id
    canMotorInterface canMI(interface_id, node_id1);

    //For catching a ctrl-c event
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = systemPowerOff;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    //ROS CONFIGURTIONS
    //Configurations of the node
    ros::init(argc, argv, "example_node");
    ros::NodeHandle n("~");
    ros::Rate loop_rate(100);

    //Subscribe to /cmd_vel
    ros::Subscriber sub = n.subscribe("/cmd_vel", 10, updateTwistSpeed);

    ros::Duration TWIST_TIMEOUT = ros::Duration(0.1);
    start_time = ros::Time::now() - TWIST_TIMEOUT;    
    
    //Fazer um write e ler com o candump
    //canMI.canTestWrite();
    //canMI.canRead();
    
    while (ros::ok()) {
        //Check if master is running
        ros::spinOnce();

        if((ros::Time::now() - start_time) > TWIST_TIMEOUT) {
            std::cout << "No messages received" << std::endl;
            count = 0;
        }

        loop_rate.sleep();
    }
}
