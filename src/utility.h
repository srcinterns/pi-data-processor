#include <stdlib.h>
#include <stdint.h>

#include "radar_config.h"

void s16_to_rdata_array(rdata_t * target, size_t size, int offset, int stride, int16_t * buffer);
