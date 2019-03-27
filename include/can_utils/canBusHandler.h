//Abstraction layer for the CAN protocol

class canBusHandler {
private:


public:
	//Criação das sockets para comunicar é feita no construtor do handler
	canBusHandler(char* can_interface);

	//char readCanFrame();
	//void writeCanFrame();
}