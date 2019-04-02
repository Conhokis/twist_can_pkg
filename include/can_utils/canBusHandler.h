//Abstraction layer for the CAN protocol
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>


#define MAXSOCK 16

class canBusHandler {
private:
	const char* _can_interface;
	int read_s;
	int write_s;
	struct sockaddr_can *addr_write;
	struct sockaddr_can *addr_read;
	

public:
	//Criação das sockets para comunicar é feita no construtor do handler
	canBusHandler(const char* can_interface);

	can_frame readCanFrame();
	void writeCanFrame(char* str_frame);
};
