#include "canBusHandler.h"
#include <stdint.h>

//Esta classe faz a ponte entre as acções da main e a tradução para mensagens de can
class canMotorInterface {
private:
	//TODO: Create object in this constructor and the constructor of the other object initializes the can interface
	//canBusHandler can_handler;
	unsigned int _node_id;
	canBusHandler* canBH;
	
public:
	canMotorInterface(const char* canInterface, const unsigned int node_id); 

	void shutdownMotor();
	void powerOnMotor();

	void canTestRead();
	void canTestWrite();
};

