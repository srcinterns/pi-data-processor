#ifndef _NETWORK_H_
#define _NETWORK_H_

typedef struct
{
	size_t size;
	// TODO: Fill with other data in the packet
	unsigned crc;
} packet_t;



int net_init(char * address, char * port); // Initialize the sending socket. Returns nonzero if an error occurred.
int net_close(void);                        // Clean up the networking nicely

int net_send_pk(packet_t * pack); // Sends a packet. Returns number of bytes sent, -1 on error
int net_send_data(); // TODO: Determine args to this function

int net_build_pk(packet_t * pack); // TODO: Fill in args to this function

#endif
