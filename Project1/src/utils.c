#include "utils.h"

int STOP_MACHINE = 0;

void sendControlMsg(int fd, unsigned char controlField)
{
    unsigned char msg[5];
    msg[0] = FLAG;
    msg[1] = A_TRM;
    msg[2] = controlField;
    msg[3] = (A_TRM || controlField);
    msg[4] = FLAG;
    write(fd, msg, 5);
}

void stateMachine(int *state, unsigned char c, char controlField)
{

    switch (*state)
    {
    case START:
        if (c == FLAG)
        {
            *state = FLAG_RCV;
        }
        break;
    case FLAG_RCV:
        if (c == A_TRM)
            *state = A_RCV;
        else
        {
            if (c == FLAG)
                *state = FLAG_RCV;
            else
                *state = START;
        }
        break;
    case A_RCV:
        if (c == controlField)
            *state = C_RCV;
        else
        {
            if (c == FLAG)
                *state = FLAG_RCV;
            else
                *state = START;
        }
        break;
    case C_RCV:
        if (c == (A_TRM || controlField)) //BCC = A_TRM ^ C
            *state = BCC_OK;
        else
            *state = START;
        break;
    case BCC_OK:
        if (c == FLAG)
        {
            *state = STOP;
            STOP_MACHINE = 1;
        }
        else
            *state = START;
        break;
    }
}

unsigned char calculateBCC2(const unsigned char *buffer, unsigned int size){
    unsigned char BCC2 = 0;
    
    for(unsigned int i=0;i<size;i++){
        BCC2 |= buffer[i];
    }
    return BCC2;
}


//Ao passarmos um buffer unstuffed e o seu tamanho calcula tamanho do bufferStuffed
int calculateSize(char* buffer, int size){
    int counter = 0;

    for(int i=0; i<size; i++){
        if(buffer[i] == 0x7e){
            counter++;
        }else if(buffer[i] == 0x7d){
            counter++;
        }
        counter++;
    }
    return counter;
}

char* stuffingData(char* buffer, int sizeWithStuffing){
    
    char stuffedBuffer[sizeWithStuffing];

    for(int i=0; i < sizeWithStuffing; i++){
        if(buffer[i] == FLAG){
            stuffedBuffer[i] = ESCAPEMENT;
            stuffedBuffer[i+1] = REPLACE_FLAG;
            i = i+2;
        }
        else if(stuffedBuffer[i] == ESCAPEMENT){
            stuffedBuffer[i] = ESCAPEMENT;
            stuffedBuffer[i+1] = REPLACE_ESCAPEMENT;
            i = i+2;
        }
        else{
            stuffedBuffer[i] = buffer[i];
            i = i+1;
        }
    }

    char* sb = stuffedBuffer;
    return sb;

}
void unstuffingData(char* buffer, int size){

    
}

