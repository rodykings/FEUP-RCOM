#include "receiver.h"

void setReceiver(int fd)
{

    stateMachine(fd, C_SET, S);
    
    printf("\nTrama SET recebida\n");
    sendControlMsg(fd, C_UA);
    printf("\nTrama UA enviada\n");
} 

int receiveControlPackage(int fd){

/*
    int state = START;
    unsigned char info;

    do{
        read(fd, &info, 1); 

    }while(state!=STOP);
  */  



};