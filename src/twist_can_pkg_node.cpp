#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <ros/time.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <lib.h>
#include <lib.c>

#include <stdint.h>

#include <sys/socket.h> /* for sa_family_t */
#include <linux/can/error.h>

#include "lib.h"

//TODO
//Run CAN interface on a separate thread, on a library with executable commands.

//Time in ns for the topic /cmd_vel, currently 100 ms.
ros::Time start_time;

//FOR NOW DUE TO TELEOP KEYBOARD KEYS, torna o input num degrau
#define MIN_HITS_KEY 5
unsigned int count = 0;

void updateSpeed(const geometry_msgs::Twist::ConstPtr& msg) {
    start_time = ros::Time::now();

    if(count < 5) count++;
 
    else std::cout << "Receiving messages" << std::endl;
}

int main(int argc, char **argv) {   

    //ROS CONFIGURTIONS
    //Configurations of the node
    ros::init(argc, argv, "example_node");
    ros::NodeHandle n("~");
    ros::Rate loop_rate(30);

    ros::Duration TWIST_TIMEOUT = ros::Duration(0.1);
    start_time = ros::Time::now() - TWIST_TIMEOUT;    
    
    //Subscribe to /cmd_vel
    ros::Subscriber sub = n.subscribe("/cmd_vel", 10, updateSpeed);
 
    //------------------------------------------

    //CAN CONFIGURATIONS
    int s; /* can raw socket */ 
    int nbytes;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

    int i;
        
    /* CAN message to be sent out */
    char buff[] = "7DF#0201050000000000";

    fprintf(stderr,"CAN testing\n");
    
    /* parse CAN frame */
    if (parse_canframe(buff, &frame)){
        fprintf(stderr, "\nWrong CAN-frame format!\n\n");
        fprintf(stderr, "Try: <can_id>#{R|data}\n");
        fprintf(stderr, "can_id can have 3 (SFF) or 8 (EFF) hex chars\n");
        fprintf(stderr, "data has 0 to 8 hex-values that can (optionally)");
        fprintf(stderr, " be seperated by '.'\n\n");
        fprintf(stderr, "e.g. 5A1#11.2233.44556677.88 / 123#DEADBEEF / ");
        fprintf(stderr, "5AA# /\n     1F334455#1122334455667788 / 123#R ");
        fprintf(stderr, "for remote transmission request.\n\n");
        return 1;
    }

    /* open socket */
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("socket");
        return 1;
    }

    addr.can_family = AF_CAN;

    strcpy(ifr.ifr_name, "can0");
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX");
        return 1;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    /* disable default receive filter on this RAW socket */
    /* This is obsolete as we do not read from the socket at all, but for */
    /* this reason we can remove the receive list in the Kernel to save a */
    /* little (really a very little!) CPU usage.                          */
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    while (ros::ok()) {
        //Check if master is running
        ros::spinOnce();


        if((ros::Time::now() - start_time) > TWIST_TIMEOUT) {
            std::cout << "No messages received" << std::endl;

            if ((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) {
                perror("write");
                return 1;
            }   

            count = 0;
        }

        loop_rate.sleep();
    }
}