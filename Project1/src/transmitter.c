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

        int * size = malloc(sizeof(int));
        stateMachine(fd, C_UA, S, size);

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

int sendControlPackage(int fd, unsigned char *controlPackage, int* size, unsigned char bcc2, int s)
{

/*
 *  Trama I : FLAG | A | C | BCC1 | Dados (pacote controlo) | BCC2 | FLAG 
 */

    int bufferSize = *size + 6;
    unsigned char buffer[bufferSize];

    int counter = 0;
    buffer[counter++] = FLAG;
    buffer[counter++] = A_TRM;
    if (s == 0)
    {
        buffer[counter++] = 0x00;
    }
    else
    {
        buffer[counter++] = 0x40;
    }
    buffer[counter++] = buffer[1] ^ buffer[2]; //bcc

    //SEND CONTROL PACKAGE HERE
    for (int i = 0; i < (*size); i++)
    {
        buffer[counter++] = controlPackage[i];
    }

    buffer[counter++] = bcc2; //bcc2;
    buffer[counter++] = FLAG;

    write(fd, &buffer,bufferSize);

    return counter;
}

unsigned char *generateControlPackage(int fileSize, unsigned char *fileName, int*packageSize)
{
    int sizeFileName = strlen(fileName);
    int packSize = 9*sizeof( unsigned char) + sizeFileName; //C,T1,L1,T2,L2(5) + sizeof(fileName) + tamanho campo filesize(4)
    unsigned char* controlPackage = (unsigned char*)(malloc(packSize));

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
    controlPackage[7] = T2; //filename
    controlPackage[8] = sizeFileName;
    for (int i = 0; i < sizeFileName; i++)
    {
        controlPackage[9 + i] = fileName[i];
    }

    *packageSize = packSize;

    return controlPackage;
}

unsigned char *sendData(int fd, unsigned char *buffer, int size, int seqN)
{
    unsigned char info[MAX_SIZE];

    //Cálculo nr tramas
    int nTramas;

    int l1 = size / 256;
    int l2 = size % 256;

    nTramas = l1;
    if (l2 != 0)
    {
        nTramas++;
    }
     
    // printf("Nr tramas: %d\n", nTramas);
    // printf("L1: %d\n", l1);
    // printf("L2: %d\n", l2);

    int counter = 0;

    info[counter++] = FLAG;
    info[counter++] = A_TRM;

    for(int i=0; i< nTramas; i++){
        //send
        if(seqN == 0){
            info[counter++] = 0x00;
        }else{
            info[counter++] = 0x40;
        }
        
        //BCC
        info[counter++] = info[1]^info[2];

        int * dataPackageSize = malloc(sizeof(int));

        unsigned char* dataPackage = generateDataPackage(buffer, dataPackageSize, i, l1, l2);

        //BCC2
        unsigned char bcc2 = calculateBCC2(dataPackage, dataPackageSize);
        
        //stuffing
        unsigned char* stuffedData = stuffingData(dataPackage,  dataPackageSize);

        for(int i=0;i<(*dataPackageSize); i++){
            info[counter++] = stuffedData[i];
        }        

        info[counter++] = bcc2;
        info[counter++] = FLAG;

        
        size = *dataPackageSize + 6;

        unsigned char  data[size];
        
        /*for(int i=0; i< size;i++){
            data[i] = info[i];
            printf(":%x", data[i]);
        }
        printf("\n");*/

        write(fd, data, size);

        (seqN == 0) ? seqN++: seqN--;
    }
    

}

unsigned char * generateDataPackage(unsigned char *buffer, int* size, int n, int l1, int l2){
    char dataPackage[MAX_SIZE];

    dataPackage[0] = C_DATA;
    dataPackage[1] = n;
    dataPackage[2] = l1;
    dataPackage[3] = l2;

    int counter = 4;
    int dataSize = 0;
    //ultima trama
    if(n == l1){
        dataSize = l2/8;
    }else{
        for(int i=n*8; i<n*8+32; i++){
            dataPackage[counter++] = buffer[i];
        }
        dataSize = 32;
    }
    
    int cnt = 0;
    unsigned char* dp = malloc((4+dataSize)*sizeof(char));
    for(int i=0; i<4+dataSize;i++){
        printf(":%x", dp[cnt]);
        dp[cnt] = dataPackage[i];
    }
    printf("\n");

    return dp;
}