#include "utils.h"

extern int alarmFlag;

int getFileSize(FILE *file)
{
    // saving current position
    long int currentPosition = ftell(file);

    // seeking end of file
    if (fseek(file, 0, SEEK_END) == -1)
    {
        printf("ERROR: Could not get file size.\n");
        return -1;
    }

    // saving file size
    long int size = ftell(file);

    // seeking to the previously saved position
    fseek(file, 0, currentPosition);

    // returning size
    return size;
}

void sendControlMsg(int fd, unsigned char header, unsigned char controlField)
{
    unsigned char msg[5];
    msg[0] = FLAG;
    msg[1] = header;
    msg[2] = controlField;
    msg[3] = (A_TRM ^ controlField);
    msg[4] = FLAG;
    write(fd, msg, 5);
}

unsigned char *stateMachine(int fd, unsigned char header, char controlField, int type, int *size)
{

    State_Machine state = START;
    unsigned char *message = malloc(sizeof(unsigned char) * MAX_SIZE);
    unsigned char c;
    int counter = 0;
    int seqN = 0;
    unsigned char* res = malloc(sizeof(unsigned char));
    res[0] = 0x3;

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
            if (c == header)
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
            if (type == S)
            {
                //ACK
                if (c == controlField)
                {
                    state = C_RCV;
                }
                else
                {
                    

                    if (c == FLAG)
                        state = FLAG_RCV;
    
                    //REJ
                    else if (c == 0x81 || c == 0x01)
                    {
                        res[0] = 0x1;
                        return res;
                    }
                    else
                    {
                        res[0] = 0x0; 
                        return res;
                    }
                }
            }
            else if (type == I)
            {
                if (c == 0x00)
                {
                    controlField = 0x00;
                    seqN = 0;
                    state = C_RCV;
                }
                else if (c == 0x40)
                {
                    seqN = 1;
                    state = C_RCV;
                    controlField = 0x40;
                }
                else
                {
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
                if (type == I)
                {

                    *size = counter;

                    message = destuffingData(message, size);

                    unsigned char bcc2 = message[*size - 1];

                    int sizeBcc = *size - 1;
                    unsigned char calcBcc2 = calculateBCC2(message, sizeBcc);

                    if (bcc2 == calcBcc2)
                    {
                        unsigned char positiveACK; // R0000101 -> 0 ou 1

                        if (seqN == 0)
                        {
                            positiveACK = 0x05;
                        }
                        else
                        {
                            positiveACK = 0x85;
                        }
                        sendControlMsg(fd, A_TRM, positiveACK);
                    }
                    else
                    {
                        unsigned char negativeACK; // R0000001 -> 0 ou 1

                        if (seqN == 0)
                        {
                            negativeACK = 0x01;
                        }
                        else
                        {
                            negativeACK = 0x81;
                        }
                        sendControlMsg(fd, A_TRM, negativeACK);
                    }
                }
                state = STOP;
            }
            else
            {
                if (type == S)
                {
                    state = START;
                }
                else
                {
                    message[counter++] = c;
                    if(counter == MAX_SIZE){
                        counter = 0;
                        state = START;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    if (type == I)
    {
        unsigned char *data = malloc(sizeof(unsigned char) * (*size));
        for (int i = 0; i < (*size); i++)
        {
            data[i] = message[i];
        }
        return data;
    }
    else
    {
        return res;
    }
}

unsigned char calculateBCC2(const unsigned char *buffer, unsigned int size)
{
    unsigned char bcc2 = 0;

    for (unsigned int i = 0; i < size; i++)
    {
        bcc2 ^= buffer[i];
    }
    return bcc2;
}

//Ao passarmos um buffer unstuffed e o seu tamanho calcula tamanho do bufferStuffed
int calculateStuffedSize(unsigned char *buffer, int size)
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

unsigned char *stuffingData(unsigned char *buffer, int *size)
{
    int startStuffedSize = *size;
    if (*size < MAX_SIZE)
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
        else if (buffer[i] == ESCAPEMENT)
        {
            stuffedBuffer[counter++] = ESCAPEMENT;
            stuffedBuffer[counter++] = REPLACE_ESCAPEMENT;
        }
        else
        {
            stuffedBuffer[counter++] = buffer[i];
        }
    }

    *size = counter;

    unsigned char *sb = malloc(sizeof(unsigned char) * (counter));
    for (int i = 0; i < counter; i++)
    {
        sb[i] = stuffedBuffer[i];
    }

    return sb;
}

unsigned char *destuffingData(unsigned char *buffer, int *size)
{
    int counter = 0;
    unsigned char destuffedData[MAX_SIZE];

    for (int i = 0; i < (*size); i++)
    {
        if (buffer[i] == 0x7d)
        {
            if (buffer[i + 1] == 0x5e)
            {
                destuffedData[counter++] = 0x7e;
            }
            else if (buffer[i + 1] == 0x5d)
            {
                destuffedData[counter++] = 0x7d;
            }
            i++;
        }
        else
        {
            destuffedData[counter++] = buffer[i];
        }
    }

    unsigned char *db = malloc(sizeof(unsigned char) * counter);

    for (int j = 0; j < counter; j++)
    {
        db[j] = destuffedData[j];
    }

    *size = counter;

    return db;
}
