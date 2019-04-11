#include "canBusHandler.h"
#include <stdint.h>

//Esta classe faz a ponte entre as acções da main e a tradução para mensagens de can
class canMotorInterface {
private:
	unsigned int _node_id;
	canBusHandler *_canBH;
	
	//Correctly returns a string in hex that is ready to be parsed and sent
	char* concDataId(char* str_data);
public:
	canMotorInterface(canBusHandler *canBH, unsigned int node_id); 

	void shutdownMotor();
	void powerOnMotor();

	void canTestRead();
	void canTestWrite();
};

