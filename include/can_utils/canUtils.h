#include "canBusHandler.h"

#define N_EXCLUSIONS 1

//Esta classe faz a ponte entre as acções da main e a tradução para mensagens de can
class canMotorInterface {
private:
	//TODO: Create object in this constructor and the constructor of the other object initializes the can interface
	//canBusHandler can_handler;
	unsigned int _node_id;
	canBusHandler* canBH;
	unsigned int id_excl[N_EXCLUSIONS]; //CAN-ID exclusions for ignoring messages sent by self

	//Checks the frame can-id to determine if message was sent from the client or the controller
	bool checkFrame(can_frame frame);
public:
	canMotorInterface(const char* canInterface, const unsigned int node_id); 

	void canRead();
	void canTestWrite();
};

