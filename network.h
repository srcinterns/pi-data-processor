#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdint.h>

#define PACKET_DATA_SIZE 512

#pragma pack(1)
typedef struct
{
	uint8_t messageid;
	uint32_t timestamp;
	uint8_t data[512];	
} packet_t;



int net_init(char * address, char * port); // Initialize the sending socket. Returns nonzero if an error occurred.
int net_close(void);                        // Clean up the networking nicely

int net_send_pk(packet_t * pack); // Sends a packet. Returns number of bytes sent, -1 on error

int net_send_data(uint8_t messageid, uint32_t timestamp, uint8_t * data, size_t datasize);

int net_build_pk(packet_t * pack, uint8_t messageid, uint32_t timestamp, uint8_t * data, size_t datasz);

#endif
