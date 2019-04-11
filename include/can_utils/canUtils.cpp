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
	uint8_t* data = _canBH->readCanMsg();
	printf("%x\n", data[0]);
}

void canMotorInterface::canTestWrite() {
	char buff[] = "611#4041600000000000";

	_canBH->writeCanFrame(buff);
}

void canMotorInterface::powerOnMotor() {
	printf("%s\n", concDataId("#4041600000000000"));
}

char* canMotorInterface::concDataId(char* str_data) {
	static char[20] str_buff;	

	if(_node_id > 15) sprintf(str_buff, "00%x", _node_id);
	else if(_node_id > 255) sprintf(str_buff, "0%x", _node_id);
	else sprintf(str_buff, "%x", _node_id);

	printf("%s\n", str_buff);

	strcat(str_buff, str_data);
	return str_buff;
}

#endif