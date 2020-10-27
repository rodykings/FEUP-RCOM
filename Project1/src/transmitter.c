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

void sendControlPackage(int fd, char* controlPackage, int size, int s){

    char buffer[size];
    int counter = 0;

    buffer[counter++] = FLAG;
    buffer[counter++] = A_TRM;
    if(s==0){
        buffer[counter++] = 0x0;
    }else{
        buffer[2] = 0x40;
    }
    buffer[3] = //bcc;

    for(int i=0; i<size; i++){

    }

    //buffer[3]

}

char* generateControlPackage(int fileSize, char *fileName)
{
    int sizeFileName = sizeof(fileName);
    int packageSize = 5 + sizeFileName + sizeof(fileSize);

    printf("CONTROL PACKAGE SIZE: %d", packageSize);

    char controlPackage[packageSize];

    /* controlPackage = [C,T1,L1,V1,T2,L2,V2]
    * C = 2 (start) || C=3 (end)
    * T = 0 (tamanho ficheiro) || T = 1(nome ficheiro)
    * L = tamanho campo V
    * V = valor
    */

    controlPackage[0] = C_START;
    controlPackage[1] = T1; //file size
    controlPackage[2] = L1;
    controlPackage[3] = (fileSize >> 24) & 0xFF;
    controlPackage[4] = (fileSize >> 16) & 0xFF;
    controlPackage[5] = (fileSize >> 8) & 0xFF;
    controlPackage[6] = fileSize & 0xFF;
    controlPackage[7] = T2; //filename
    controlPackage[8] = sizeFileName;
    for (int i = 0; i < sizeFileName; i++)
    {
        controlPackage[9 + i] = fileName[i];
    }

    char* cp = controlPackage;
    return cp;
}

char* generateDataPackage(int n, int l1, int l2, char *buffer)
{
    int size = 4 + 256;

    if (l1 == n)
    {
        size = 4 + l2;
    }

    char dataPackage[size];

    dataPackage[0] = C_DATA;
    dataPackage[1] = l1;
    dataPackage[2] = l2;
    //dataPackage[3] = buffer;

    char* dp = dataPackage;
    return dp;
}