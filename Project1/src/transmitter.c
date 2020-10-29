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

void sendControlPackage(int fd, char *controlPackage, int size, unsigned char bcc2, int s)
{

    /*
 *  Trama I : FLAG | A | C | BCC1 | Dados (pacote controlo) | BCC2 | FLAG 
 */

    int bufferSize = size + 6;
    char buffer[bufferSize];

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
    buffer[counter++] = buffer[1] || buffer[2]; //bcc

    //SEND CONTROL PACKAGE HERE
    for (int i = 0; i < size; i++)
    {
        buffer[counter++] = controlPackage[i];
    }

    buffer[counter++] = bcc2; //bcc2;
    buffer[counter++] = FLAG;

    write(fd, &buffer, bufferSize);
    printf("%x\n", buffer[0]);              //flag
    printf("%x\n", buffer[1]);              //a
    printf("%x\n", buffer[2]);              //c
    printf("%x\n", buffer[3]);              //bcc
    printf("%x\n", buffer[bufferSize - 2]); //bcc2
    printf("%x\n", buffer[bufferSize - 1]); //flag
}

char *generateControlPackage(int fileSize, char *fileName)
{
    int sizeFileName = sizeof(fileName);
    int packageSize = 5 + sizeFileName + sizeof(fileSize);

    printf("CONTROL PACKAGE SIZE: %d\n", packageSize);

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

    char *cp = controlPackage;
    return cp;
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