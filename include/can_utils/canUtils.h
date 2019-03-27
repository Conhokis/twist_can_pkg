#include "canBusHandler.h"
//TODO: PASSAR OS IDS DOS NODES E O NOME DA INTERFACE PELO CONSTRUTOR

//Esta classe faz a ponte entre as acções da main e a tradução para mensagens de can
class canInterface {
private:
	//TODO: Create object in this constructor and the constructor of the other object initializes the can interface
	canBusHandler can_handler;

public:
	canInterface(char* canInterface, char* node_id); 

}