#ifndef CANI_H
#define CANI_H

#include "canUtils.h"
#include <iostream>

canMotorInterface::canMotorInterface(const char* canInterface, const unsigned int node_id[]) {
	//Intitialize canBusHandler which creates and mantains the sockets
	//Save NODE ID
	_node_id = node_id;

	//Start canBusHandler, manages connection and executes read and write functions
	canBH = new canBusHandler(canInterface);
}

void canMotorInterface::shutdownMotor() {
	canBH->writeCanFrame("611#2B40600000000000");
}

void canMotorInterface::canTestRead() {
	/*
	struct can_frame buff_frame;

	buff_frame = canBH->readCanFrame();

	while(checkFrame(buff_frame)) {
		buff_frame = canBH->readCanFrame();
	}*/

	static uint8_t* data = canBH.readCanMsg();
}

void canMotorInterface::canTestWrite() {
	char buff[] = "611#4041600000000000";

	canBH->writeCanFrame(buff);
}



#endif