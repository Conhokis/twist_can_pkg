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
	_canBH->writeCanFrame((char*) "611#2B40600000000000");
}

void canMotorInterface::checkMotorStatus() {
	std::cout << "Checking for motors..." << std::endl;

	//Read status word, 0x600 indicates this is an SDO (Service Data Object)
	_canBH->writeCanFrame(concDataId((char*) "#4041600000000000", 0x600));
	//uint8_t *read_data = _canBH->readCanMsg();
	_canBH->readCanMsg();

	std::cout << "Motors found! Starting motor power up." << std::endl;
}

void canMotorInterface::powerOnMotor() {
	//Set velocity mode
	_canBH->writeCanFrame(concDataId((char*) "#2F60600002000000", 0x600));

	//Check if velocity mode is set
	uint8_t *read_data;
	do {
		_canBH->writeCanFrame(concDataId((char*) "#4061600000000000", 0x600));
		read_data = _canBH->readCanMsg();
	} while(read_data[4] != 0x02); //Byte 5 is the start of the data in the frame

	//Set target velocity to 0
	_canBH->writeCanFrame(concDataId((char*) "#2B426000C8000000", 0x600));

	//Set motor ready to switch on
	_canBH->writeCanFrame(concDataId((char*) "#2B40600006000000", 0x600));

	//Check if motor is ready to switch on
	do {
		_canBH->writeCanFrame(concDataId((char*) "#4041600000000000", 0x600));
		read_data = _canBH->readCanMsg();
	} while(((read_data[4] & 0b00100001) != 33) || ((read_data[5] & 0b00000010) != 2));

	//Switch on motor
	_canBH->writeCanFrame(concDataId((char*) "#2B40600007000000", 0x600));

	//Check if motor is switched on
	do {
		_canBH->writeCanFrame(concDataId((char*) "#4041600000000000", 0x600));
		read_data = _canBH->readCanMsg();
	} while(((read_data[4] & 0b00110011) != 51) || ((read_data[5] & 0b00000010) != 2));

	//Strt operating mode
	_canBH->writeCanFrame(concDataId((char*) "#2B4060000F000000", 0x600));

	//Check if motor is in operating mode
	do {
		_canBH->writeCanFrame(concDataId((char*) "#4041600000000000", 0x600));
		read_data = _canBH->readCanMsg();
	} while(((read_data[4] & 0b00110111) != 55) || ((read_data[5] & 0b00000010) != 2));

	std::cout << "Motor is ready for operation!" << std::endl;
}

void canMotorInterface::setTargetVelocity(int16_t target_vel) {
	char final_buff[21];
	sprintf(final_buff, "%03x#2B426000", _node_id + 0x600);

	char str_buff[9], le_str_buff[9];
	//Convert to string and order the bytes to little endian
	sprintf(str_buff, "%08x", target_vel);
	le_str_buff[7] = str_buff[1];
	le_str_buff[6] = str_buff[0];
	le_str_buff[5] = str_buff[3];
	le_str_buff[4] = str_buff[2];
	le_str_buff[3] = str_buff[5];
	le_str_buff[2] = str_buff[4];
	le_str_buff[1] = str_buff[7];
	le_str_buff[0] = str_buff[6];

	strcat(final_buff, str_buff);

	printf("%s\n", final_buff);
}

char* canMotorInterface::concDataId(char* str_data, unsigned int can_cmd) {
	static char str_buff[21];	
	sprintf(str_buff, "%03x", _node_id + can_cmd);
	strcat(str_buff, str_data);
	return str_buff;
}

#endif