//Abstraction layer for the CAN protocol

#define MAXSOCK 16

class canBusHandler {
private:
	const char* _can_interface;
	int s[MAXSOCK];
	

public:
	//Criação das sockets para comunicar é feita no construtor do handler
	canBusHandler(const char* can_interface);

	//char readCanFrame();
	//void writeCanFrame();
};