#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <stdint.h>

#include "network.h"

#define MAX(a,b) ((a>b)?a:b)

int sockfd = -1;
struct sockaddr target;
socklen_t targetlen = sizeof(struct sockaddr);


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
		if (-1 != (sockfd = socket(p->ai_family, p->ai_family, p->ai_protocol)))
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

	if (-1 == (ret = sendto(sockfd, pack, sizeof(packet_t), 0, &target, targetlen))) {
		perror("net_send_pk");
	}

	return ret;	
}

int net_send_data(uint32_t segment, uint16_t index)
{
	packet_t out;

	out.segment = htonl(segment);
	out.index = htons(index);

	net_send_pk(&out);	

	return 0;
}
/*
int net_build_pk(packet_t * pack, uint16_t segmentid, uint8_t total, uint8_t seq, uint8_t * data, uint32_t datasz)
{
	// Load all the args into the struct here
	pack->segmentid = segmentid;
	pack->total = total;
	pack->seq = seq;
	pack->size = (uint16_t) datasz;
	memcpy(pack->data, data, datasz);

	return 0;
}
*/
