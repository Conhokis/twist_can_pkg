#ifndef CANI_H
#define CANI_H

#include "canUtils.h"
#include <iostream>

canMotorInterface::canMotorInterface(canBusHandler *canBH, unsigned int node_id) {
	_node_id = node_id;
	_canBH = canBH;
}

void canMotorInterface::shutdownMotor() {
	_canBH->writeCanFrame("611#2B40600000000000");
}

void canMotorInterface::canTestRead() {
	/*
	struct can_frame buff_frame;

	buff_frame = _canBH->readCanFrame();

	while(checkFrame(buff_frame)) {
		buff_frame = _canBH->readCanFrame();
	}*/

	static uint8_t* data = _canBH->readCanMsg();
}

void canMotorInterface::canTestWrite() {
	char buff[] = "611#4041600000000000";

	_canBH->writeCanFrame(buff);
}



#endif