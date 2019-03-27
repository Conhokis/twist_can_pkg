//Abstraction layer for the CAN protocol

#define MAXSOCK 16

class canBusHandler {
private:
	const char* _can_interface;
	int read_s;
	int write_s;
	struct sockaddr_can *addr_write;

public:
	//Criação das sockets para comunicar é feita no construtor do handler
	canBusHandler(const char* can_interface);

	void readCanFrame();
	//void writeCanFrame();
};