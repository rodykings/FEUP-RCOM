#include "transmitter.h"

int numRetry = 0;
int alarmFlag = FALSE;

int setTransmitter(int fd)
{
    (void)signal(SIGALRM, alarmHandler);

    int state = START;
    unsigned char c;

    do
    {
        sendControlMsg(fd, C_SET);
        printf("Trama SET enviada\n");

        alarmFlag = FALSE;
        alarm(TIMEOUT);

        while (state != STOP && !alarmFlag)
        {
            read(fd, &c, 1);
            stateMachine(&state, c, C_UA);
        }
    } while (alarmFlag && numRetry < MAX_RETRY);

    //Comunicação falha após atingir o número máximo de tentativas
    if (alarmFlag && numRetry == MAX_RETRY)
        return FALSE;

    else
    {
        printf("Trama UA recebida\n");
        alarmFlag = FALSE;
        numRetry = 0;
        printf("\nProtocol connection established!\n");
        return TRUE;
    }
}


int sendControlPackage(int fd, int fileSize, char* fileName){


   int sizePath = sizeof(fileName);
   int packageSize = 8*5 + sizePath + fileSize;

    int pos = 0;

   unsigned char controlPackage[packageSize];

   controlPackage[pos++] = C_START;
   controlPackage[pos++] = T1;
   controlPackage[pos++] = fileSize;

/*
	for (unsigned int i = 0; i < strlen(fileSize); i++)
		controlPackage[pos++] = char(fileSize[i]);

    write(fd, &controlPackage, packageSize);
*/
    return 0;
}