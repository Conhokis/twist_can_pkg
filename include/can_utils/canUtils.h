#include "canBusHandler.h"
#include <stdint.h>

//Esta classe faz a ponte entre as acções da main e a tradução para mensagens de can
class canMotorInterface {
private:
	unsigned int _node_id;
	canBusHandler *_canBH;
	
	//Correctly returns a string in hex that is ready to be parsed and sent
	char* concDataId(char* str_data, unsigned int can_cmd);
	char* hexToByteLE(char* hex_str);
public:
	canMotorInterface(canBusHandler *canBH, unsigned int node_id); 

	//Powers off motor and engage break
	void shutdownMotor();
	//Powers on motor and starts operating mode
	void powerOnMotor();
	//To check if the motor is connected
	void checkMotorStatus();
	//Set target velocity
	void setTargetVelocity(int16_t target_vel);
	//Reads the motor speed
	int32_t readMotorEncoder();
	//Reads and stores the encoder resolution for odometry
	int32_t getEncoderResolution();
};

