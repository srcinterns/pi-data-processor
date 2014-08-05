#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdint.h>

#define MAX_DATA_SIZE 512

// Effectively disable struct packing, to ensure there is no padding
#pragma pack(1)
typedef struct
{
	uint16_t segmentid;
	uint8_t total;
	uint8_t seq;
	uint16_t size;
	uint8_t data[MAX_DATA_SIZE];	
} packet_t;



int net_init(char * address, char * port); // Initialize the sending socket. Returns nonzero if an error occurred.
int net_close(void);                        // Clean up the networking nicely

int net_send_pk(packet_t * pack); // Sends a packet. Returns number of bytes sent, -1 on error

int net_send_data(uint8_t * data, uint32_t datasz);

int net_build_pk(packet_t * pack, uint16_t segmentid, uint8_t total, uint8_t seq, uint8_t * data, uint32_t datasz);

#endif
