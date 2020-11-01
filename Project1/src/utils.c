#include "utils.h"

extern int alarmFlag;

void sendControlMsg(int fd, unsigned char controlField)
{
    unsigned char msg[5];
    msg[0] = FLAG;
    msg[1] = A_TRM;
    msg[2] = controlField;
    msg[3] = (A_TRM ^ controlField);
    msg[4] = FLAG;
    write(fd, msg, 5);
}

unsigned char* stateMachine(int fd, char controlField, int type, int* size)
{

    State_Machine state = START;
    unsigned char* message = malloc(sizeof(char)*MAX_SIZE);
    unsigned char c;
    int counter = 0;
    int seqN = 0;

    while (state != STOP && !alarmFlag)
    {
        read(fd, &c, 1);

        switch (state)
        {
        case START:
            if (c == FLAG)
            {
                state = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (c == A_TRM)
            {
                state = A_RCV;
            }
            else
            {
                if (c == FLAG)
                    state = FLAG_RCV;
                else
                    state = START;
            }
            break;
        case A_RCV:
            if(type == S){
                //ACK
                //printf("Control field: %x\n", controlField);
                if (c == controlField)
                {   
                    //printf("ACK HERE\n");
                    state = C_RCV;
                }
                else
                {
                    if (c == FLAG)
                        state = FLAG_RCV;
                    //REJ
                    else if(c == 0x81 || c == 0x01){
                        return NULL;
                    }
                    else{
                        return NULL;
                    }
                        
                }
            }else if(type == I){
                if(c == 0x00 ){
                    seqN=0;
                    state = C_RCV;
                }else if(c==0x40){
                    seqN=1;
                    state = C_RCV;
                }else{
                     if (c == FLAG)
                        state = FLAG_RCV;
                    else
                        state = START;
                }
            }
            
            break;
        case C_RCV:
            if (c == (A_TRM ^ controlField)) //BCC = A_TRM ^ C
            { 
                state = BCC_OK;
            }
            else
                state = START;
            break;
        case BCC_OK:
            if (c == FLAG)
            {
                if(type == I){
                    unsigned char bcc = message[counter-1];
                    *size = counter-2;
                    destuffingData(message, size);

                    
                    if(bcc == calculateBCC2(message, size)){
                        unsigned char positiveACK; // R0000101 -> 0 ou 1

                        if(seqN == 0){
                            positiveACK = 0x05;
                        }else{
                            positiveACK = 0x85;
                        }
                        printf("Positive ACK sent: %x\n", positiveACK);
                        sendControlMsg(fd, positiveACK);
                        printf("Trama RR enviada!\n");
                    }
                    else{
                        unsigned char negativeACK; // R0000001 -> 0 ou 1

                        if(seqN == 1){
                            negativeACK = 0x81;
                        }else{
                            negativeACK = 0x01;
                        }
                        //printf("Negative ACK sent: %x\n", negativeACK);
                        sendControlMsg(fd, negativeACK);
                        printf("Trama RJ enviada!\n");
                    }  
                }
                state = STOP;
            }
            else{
                if(type == S){
                    state = START;
                }else{
                    message[counter++] = c;
                } 
            }  
            break;
        default:
            break;
        }
        
    }

    if(type == I){
            *size = counter;
            unsigned char* data = malloc(sizeof(char)*(counter+1));
            for(int i=0; i<counter; i++){
                data[i] = message[i];
            }return data;
    }else {
        //TODO
        unsigned char* akn = "A";
        return akn;
    }

}

unsigned char calculateBCC2(const unsigned char *buffer, unsigned int* size)
{
    unsigned char bcc2 = 0;

    for (unsigned int i = 0; i < (*size); i++)
    {
        bcc2 ^= buffer[i];
    }
    return bcc2;
}

//Ao passarmos um buffer unstuffed e o seu tamanho calcula tamanho do bufferStuffed
int calculateStuffedSize(char *buffer, int size)
{
    int counter = 0;

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] == 0x7e)
        {
            counter++;
        }
        else if (buffer[i] == 0x7d)
        {
            counter++;
        }
        counter++;
    }
    return counter;
}

unsigned char *stuffingData(char *buffer, int* size)
{
    int startStuffedSize = *size; 
    if(*size < MAX_SIZE)
        startStuffedSize = MAX_SIZE;
    
    int counter = 0;
    unsigned char stuffedBuffer[startStuffedSize];

    for (int i = 0; i < (*size); i++)
    {
        if (buffer[i] == FLAG)
        {
            stuffedBuffer[counter++] = ESCAPEMENT;
            stuffedBuffer[counter++] = REPLACE_FLAG;
        }
        else if (stuffedBuffer[i] == ESCAPEMENT)
        {
            stuffedBuffer[counter++] = ESCAPEMENT;
            stuffedBuffer[counter++] = REPLACE_ESCAPEMENT;
        }
        else
        {
            stuffedBuffer[counter++] = buffer[i];
        }
    }


    unsigned char* sb = malloc(sizeof(unsigned char)*(counter+1));
    for(int i=0; i<counter;i++){
        sb[i] = stuffedBuffer[i];
    }return sb;
}
unsigned char* destuffingData(char *buffer, int* size)
{
    int counter = 0;
    unsigned char destuffedData[MAX_SIZE];

    for(int i=0; i<(*size); i++){
        if(buffer[i] == 0x7d){
            if(buffer[i+1] == 0x5e){
                destuffedData[counter++] = 0x7e;
            }else if(buffer[i+1] == 0x5d){
                destuffedData[counter++] = 0x7d;
            }
            i++;
        }else {
            destuffedData[counter++] = buffer[i];
        }
    }

    char* db = malloc(sizeof(char)*counter+1);

    for(int j=0; j<counter; j++){
        db[j] = destuffedData[j];
    }

    counter++;
    *size = counter;

    return db;
    
}
