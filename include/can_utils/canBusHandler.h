//Abstraction layer for the CAN protocol
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdint.h>

#define MAXSOCK 16

class canBusHandler {
private:
	const char* _can_interface;
	int read_s;
	int write_s;
	struct sockaddr_can *addr_write;
	struct sockaddr_can *addr_read;

	unsigned int *id_excl; //CAN-ID exclusions for ignoring messages sent by self
	unsigned int N_EXCLUSIONS;
	
	//Reads a can frame from the bus, there are protections with checkFrame so it excludes
	//the frame when it reads what we write to the bus
	can_frame readCanFrame();
	//Checks the frame can-id to determine if message was sent from the client or the controller
	bool checkFrame(can_frame frame);

public:
	//Criação das sockets para comunicar é feita no construtor do handler
	canBusHandler(const char* can_interface, unsigned int node_id[]);

	//Reads the bus and returns a string representation of the CAN message
	uint8_t* readCanMsg();
	//Writes the CAN frame to the bus, the 
	void writeCanFrame(char* str_frame);
};
