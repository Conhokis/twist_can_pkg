#ifndef CANI_H
#define CANI_H

#include "canUtils.h"
#include <iostream>

canMotorInterface::canMotorInterface(const char* canInterface, const char* node_id) {
	//Intitialize canBusHandler which creates and mantains the sockets
	//Save NODE ID
	_node_id = node_id;

	//Add exclusions. Page 98 of C5-E motor controller manual.
	id_excl = {600};

	//Start canBusHandler, manages connection and executes read and write functions
	canBH = new canBusHandler(canInterface);
}

void canMotorInterface::canRead() {
	struct can_frame buff_frame;

	buff_frame = canBH->readCanFrame();

	while(checkFrame(buff_frame)) {
		buff_frame = canBH->readCanFrame();
	}

	printf("%x\n", buff_frame.can_id);
}

void canMotorInterface::canTestWrite() {
	char buff[] = "611#4041600000000000";

	canBH->writeCanFrame(buff);
}

bool canMotorInterface::checkFrame(can_frame frame) {
	bool check = false;
	for(i = 0; i < N_EXCLUSIONS; i++) {
		if(frame.can_id == id_excl) {
			check = true;
			break;
		}
	}

	return check;
}

#endif