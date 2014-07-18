#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>

#include "network.h"

int sockfd = -1;
struct sockaddr target;
socklen_t targetlen = sizeof(struct sockaddr);

static unsigned calc_crc(char * buffer, size_t size)
{
	int i;
	unsigned ret = 17;

	if (NULL == buffer) {
		fprintf(stderr,"crc: NULL buffer received\n");
		return ~0x0;
	}

	for (i = 0; i < size; i++) {
		ret ^= buffer[i];
	}

	return ret;
}


int net_init(char * address, char * port)
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	
	memset(&hints,0,sizeof(hints));


	if ((NULL == address) || (NULL == port))
	{
		fprintf(stderr,"Invalid address or port\n");
		return -1;
	}


    if ((rv = getaddrinfo(address,port,&hints,&servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -2;
    }

	for (p = servinfo;  p != NULL; p = p->ai_next)
	{
		if (-1 != (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)))
			break;
	}
	if (NULL == p)
	{
		fprintf(stderr, "Could not create a socket!\n");
		return -3;
	}

	memcpy(&target, p->ai_addr, sizeof(struct sockaddr));

	freeaddrinfo(servinfo);

	return 0;
}


int net_close(void)
{
	return close(sockfd);
}

int net_send_pk(packet_t * pack)
{
	int ret = 0;

	if (NULL == pack) {
		fprintf(stderr,"net_send_pk: Packet is null!\n");
		return -1;
	}

	// NOTE: This needs to be changed if the size of the packet is variable
	if (-1 == (ret = sendto(sockfd, pack, sizeof(pack), 0, &target, targetlen))) {
		perror("net_send_pk");
	}

	return ret;	
}

// TODO: Put the args needed to build a packet in here
int net_send_data()
{
	packet_t out;

	net_build_pk(&out);
	net_send_pk(&out);

	return 0;
}

// TODO: Determing args to this function
int net_build_pk(packet_t * pack)
{
	// Load all the args into the struct here

	pack->crc = calc_crc((char*) pack, sizeof(packet_t) - sizeof(unsigned));

	return 0;
}

