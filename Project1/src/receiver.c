#include "receiver.h"

void setReceiver(int fd)
{
    unsigned char c;
    int state = START;

    while (state != STOP)
    {
        read(fd, &c, 1); /* returns after 1 chars have been input */
        stateMachine(&state, c, C_SET);
    }
    printf("Trama SET recebida\n");
    sendControlMsg(fd, C_UA);
    printf("Trama UA enviada\n");
}