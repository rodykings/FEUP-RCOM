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
        printf("\nTrama SET enviada\n");

        alarmFlag = FALSE;
        alarm(TIMEOUT);

        stateMachine(fd, C_UA, S);

    } while (alarmFlag && numRetry < MAX_RETRY);

    //Comunicação falha após atingir o número máximo de tentativas
    if (alarmFlag && numRetry == MAX_RETRY)
        return FALSE;

    else
    {
        printf("\nTrama UA recebida\n");
        alarmFlag = FALSE;
        numRetry = 0;
        printf("\nProtocol connection established!\n");
        return TRUE;
    }
}

int sendControlPackage(int fd, unsigned char *controlPackage, int size, unsigned char bcc2, int s)
{

/*
 *  Trama I : FLAG | A | C | BCC1 | Dados (pacote controlo) | BCC2 | FLAG 
 */

    int bufferSize = size + 6;
    unsigned char buffer[bufferSize];

    int counter = 0;
    buffer[counter++] = FLAG;
    buffer[counter++] = A_TRM;
    if (s == 0)
    {
        buffer[counter++] = 0x0;
    }
    else
    {
        buffer[counter++] = 0x40;
    }
    buffer[counter++] = buffer[1] ^ buffer[2]; //bcc

    //SEND CONTROL PACKAGE HERE

    printf("SIZE: %d\n", size);
    for (int i = 0; i < size; i++)
    {
        buffer[counter++] = controlPackage[i];
    }

    buffer[counter++] = bcc2; //bcc2;
    buffer[counter++] = FLAG;

    write(fd, &buffer, bufferSize);
    printf("Flag: %x\n", buffer[0]);              //flag
    printf("A: %x\n", buffer[1]);              //a
    printf("C: %x\n", buffer[2]);              //c
    printf("BCC: %x\n", buffer[3]);              //bcc
    for(int i = 4; i<bufferSize-6; i++)
        printf("DATA: %x\n", buffer[i]);
    printf("BCC2: %x\n", buffer[bufferSize - 2]); //bcc2
    printf("FLAG: %x\n", buffer[bufferSize - 1]); //flag

    return counter;
}

unsigned char *generateControlPackage(int fileSize, unsigned char *fileName)
{
    int sizeFileName = sizeof(fileName);
    int packageSize = 5*sizeof( unsigned char) + sizeFileName + 4*sizeof( unsigned char); //C,T1,L1,L2 + sizeof(fileName) + tamanho campo filename

    printf("CONTROL PACKAGE SIZE: %d\n", packageSize);

    unsigned char* controlPackage = (unsigned char*)(malloc(packageSize));

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
    controlPackage[6] = (fileSize & 0xFF);
    
    printf("BYTE: %x\n", (unsigned char)controlPackage[6]);
    controlPackage[7] = T2; //filename
    controlPackage[8] = sizeFileName;
    for (int i = 0; i < sizeFileName; i++)
    {
        controlPackage[9 + i] = fileName[i];
    }
    
    return controlPackage;
}

char *generateDataPackage(int n, int l1, int l2, char *buffer)
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

    char *dp = dataPackage;
    return dp;
}