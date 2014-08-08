#include "utility.h"

// Copies values from buffer to target (which must be size "size"), starting at the buffer offset, and incrementing every stride
void s16_to_rdata_array(rdata_t * target, size_t size, int offset, int stride, int16_t * buffer)
{
	int i,j = 0;

	for (i = offset; j < size; j++,i+=stride) {
		target[j] = (rdata_t) buffer[i];
	}

}
