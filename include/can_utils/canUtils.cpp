#ifndef CANI_H
#define CANI_H

#include "canUtils.h"
#include <iostream>

canMotorInterface::canMotorInterface(const char* canInterface, const char* node_id1, const char* node_id2) {
	//Intitialize canBusHandler which creates and mantains the sockets
	//Save NODE ID
	_node_id1 = node_id1;
	_node_id2 = node_id2;

	canBH = new canBusHandler(canInterface);
}

void canMotorInterface::canRead() {
	canBH->readCanFrame();
}

void canMotorInterface::canTestWrite() {
	unisgned char buff[] = "7DF#0201050000000000";

	canBH->writeCanFrame(buff);
}

#endif