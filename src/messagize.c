/****************************************************************/
/* Messagize.c
 * David Campbell
 * personal email: dncswim76@gmail.com
 * work email: dcampbell@srcinc.com
 *
 * The purpose of this files is to provide a means of splitting up
 * UDP packets into carrier packets based off the MAX_MESSAGE_SIZE
 * value.                                                        */
/*****************************************************************/

#include "messagize.h"
#include "network.h"
#include <math.h>

/*MESSAGIZE AND SEND - send a message, but split into
 * carrier packets*/
void messagize_and_send(char* buffer, int size){

    int num_carriers = (int)ceilf((float)size/(float)MAX_MESSAGE_SIZE);
    int left_over_size = size - (num_carriers-1)*MAX_MESSAGE_SIZE;
    int carrier_id;

    /*send all full message packets*/
    for (carrier_id = 0; carrier_id < num_carriers-1; carrier_id++){
        net_send_data(num_carriers,
                      carrier_id,
                      &buffer[MAX_MESSAGE_SIZE*carrier_id],
                      MAX_MESSAGE_SIZE);
    }
    /*send the remaining data*/
    net_send_data(num_carriers,
                  carrier_id,
                  &buffer[MAX_MESSAGE_SIZE*carrier_id],
                  left_over_size);

}


