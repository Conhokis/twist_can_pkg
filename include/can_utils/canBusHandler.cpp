#ifndef CANBH_H
#define CANBH_H

#include "canBusHandler.h"
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

extern "C" {
	#include "lib.h"
}


extern int optind, opterr, optopt;

static volatile int running = 1;

#define ANYDEV "any"


canBusHandler::canBusHandler(const char* can_interface) {
	//Inicialização de varáveis desconhecidas para já
	//Guardar os valores necessários para a conexão, nomeadamente as sockets e assim
	_can_interface = can_interface;
	
	//Variables necessary for creating and binding to socket
	int rcvbuf_size = 0;
	int currmax;
	struct sockaddr_can addr;
	int i;
	struct ifreq ifr;
	int s[1];

	//Number of maximum concurrent can connections
	currmax = 1;

	if (currmax > MAXSOCK) {
		fprintf(stderr, "More than %d CAN devices given on commandline!\n", MAXSOCK);
	}

	for (i=0; i < currmax; i++) {

		s[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (s[i] < 0) {
			perror("socket");
		}

		addr.can_family = AF_CAN;

		memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
		strncpy(ifr.ifr_name, _can_interface, strlen(_can_interface));

		if (strcmp(ANYDEV, ifr.ifr_name)) {
			if (ioctl(s[i], SIOCGIFINDEX, &ifr) < 0) {
				perror("SIOCGIFINDEX");
				exit(1);
			}
			addr.can_ifindex = ifr.ifr_ifindex;
		} else
			addr.can_ifindex = 0; /* any can interface */

		if (rcvbuf_size) {

			int curr_rcvbuf_size;
			socklen_t curr_rcvbuf_size_len = sizeof(curr_rcvbuf_size);

			/* try SO_RCVBUFFORCE first, if we run with CAP_NET_ADMIN */
			if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUFFORCE,
				       &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {

				if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUF,
					       &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
					perror("setsockopt SO_RCVBUF");
				}

				if (getsockopt(s[i], SOL_SOCKET, SO_RCVBUF,
					       &curr_rcvbuf_size, &curr_rcvbuf_size_len) < 0) {
					perror("getsockopt SO_RCVBUF");
				}

				/* Only print a warning the first time we detect the adjustment */
				/* n.b.: The wanted size is doubled in Linux in net/sore/sock.c */
				if (!i && curr_rcvbuf_size < rcvbuf_size*2)
					fprintf(stderr, "The socket receive buffer size was "
						"adjusted due to /proc/sys/net/core/rmem_max.\n");
			}
		}

		if (bind(s[i], (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
		}

		read_s = s[i];
	}

	int s_w; /* can raw socket */ 
	int nbytes;
	struct sockaddr_can addr_w;
	struct ifreq ifr_w;

	/* open socket */
	if ((s_w = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
	}

	addr_w.can_family = AF_CAN;

	strcpy(ifr_w.ifr_name, _can_interface);
	if (ioctl(s_w, SIOCGIFINDEX, &ifr_w) < 0) {
		perror("SIOCGIFINDEX");
	}
	addr_w.can_ifindex = ifr_w.ifr_ifindex;

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	setsockopt(s_w, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	if (bind(s_w, (struct sockaddr *)&addr_w, sizeof(addr_w)) < 0) {
		perror("bind");	}

	write_s = s_w;
	*addr_write = addr_w;
}

void canBusHandler::readCanFrame() {
	struct msghdr msg;
	struct can_frame frame;
	int nbytes;
	fd_set rdfs;
	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];

	FD_ZERO(&rdfs);
	FD_SET(write_s, &rdfs);

	if (FD_ISSET(write_s, &rdfs)) {

		/* these settings may be modified by recvmsg() */
		msg.msg_namelen = sizeof(addr_write);
		msg.msg_controllen = sizeof(ctrlmsg);  
		msg.msg_flags = 0;

		nbytes = recvmsg(write_s, &msg, 0);
		if (nbytes < 0) {
			perror("read");
		}

		if (nbytes < sizeof(struct can_frame)) {
			fprintf(stderr, "read: incomplete CAN frame\n");

		fprint_long_canframe(stdout, &frame, NULL, 0);
		//Aqui tem que fazer uma cópia do frame e retornar, se for preciso depois faço umas funções para dar parse à frame baseado no lib.c
		printf("\n");
		}
	}	
}

#endif