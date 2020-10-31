#include "receiver.h"

void setReceiver(int fd)
{

    stateMachine(fd, C_SET, S);
    
    printf("\nTrama SET recebida\n");
    sendControlMsg(fd, C_UA);
    printf("\nTrama UA enviada\n");
} 

int receiveControlPackage(int fd){

    unsigned char* controlPackage = stateMachine(fd, 0x00, I);

    printf("\nTrama de controlo I recebida\n");
   // sendControlMsg(fd, 0x00); //TODO -> C vai variar consoante o tipo de RR / REJ

    return 0;
};