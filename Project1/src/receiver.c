#include "receiver.h"

void setReceiver(int fd)
{

    stateMachine(fd, C_SET, S);
    
    printf("\nTrama SET recebida\n");
    sendControlMsg(fd, C_UA);
    printf("\nTrama UA enviada\n");
} 

int receiveControlPackage(int fd, int size){

    char buffer[size];
   // stateMachine(fd, 0x00, S);  //TODO -> valor de S varia (S ou I alternados)
    printf("\nTrama de controlo I recebida\n");
   // sendControlMsg(fd, 0x00); //TODO -> C vai variar consoante o tipo de RR / REJ

    
    for(int i=0;i<size;i++){
        read(fd, &buffer[i], 1);  
        printf("Buffer: %x\n", buffer[i]);
    }
    return 0;
};