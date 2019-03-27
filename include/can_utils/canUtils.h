#include "canBusHandler.h"
//TODO: PASSAR OS IDS DOS NODES E O NOME DA INTERFACE PELO CONSTRUTOR

//Esta classe faz a ponte entre as acções da main e a tradução para mensagens de can
class canMotorInterface {
private:
	//TODO: Create object in this constructor and the constructor of the other object initializes the can interface
	//canBusHandler can_handler;
	const char* _node_id1;
	const char* _node_id2;
	canBusHandler* canBH;

public:
	canMotorInterface(const char* canInterface, const char* node_id1, const char* node_id2); 

};

