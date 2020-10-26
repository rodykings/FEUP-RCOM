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
