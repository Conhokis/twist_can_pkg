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

void canMotorInterface::checkMotorStatus() {
	std::cout << "Checking for motors..." << std::endl;

	_canBH->writeCanFrame(concDataId("#4041600000000000", 0x600));
	//uint8_t *read_data = _canBH->readCanMsg();
	_canBH->readCanMsg();

	std::cout << "Motors found! Starting motor power up." << std::endl;
}

void canMotorInterface::powerOnMotor() {
	//Set velocity mode
	_canBH->writeCanFrame(concDataId("#2F60600002000000", 0x600));

	_canBH->writeCanFrame(concDataId("#4060610000000000", 0x600));
	uint8_t *read_data;
	do {
		read_data = _canBH->readCanMsg();
	} while(read_data[4]);


}

char* canMotorInterface::concDataId(char* str_data, unsigned int can_cmd) {
	static char str_buff[20];	

	if(_node_id + can_cmd < 16) sprintf(str_buff, "00%x", _node_id + can_cmd);
	else if(_node_id + can_cmd < 256) sprintf(str_buff, "0%x", _node_id + can_cmd);
	else sprintf(str_buff, "%x", _node_id + can_cmd);

	strcat(str_buff, str_data);
	return str_buff;
}

#endif