#ifndef CANI_H
#define CANI_H

#include "canUtils.h"
#include <iostream>
#include <string.h>

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
	printf("%s\n", concDataId("#4041600000000000", 0x600));
	printf("%s\n", concDataId("#4041600000000000", 0x60));
	printf("%s\n", concDataId("#4041600000000000", 0x6));
}

char* canMotorInterface::concDataId(char* str_data, unsigned int can_cmd) {
	static char str_buff[20];	

	if(_node_id + can_cmd < 16) sprintf(str_buff, "00%x", _node_id + can_cmd);
	else if(_node_id + can_cmd < 256) sprintf(str_buff, "0%x", _node_id + can_cmd);
	else sprintf(str_buff, "%x", _node_id + can_cmd);

	printf("%s\n", str_buff);

	strcat(str_buff, str_data);
	return str_buff;
}

#endif