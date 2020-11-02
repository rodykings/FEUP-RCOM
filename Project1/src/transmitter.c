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
        sendControlMsg(fd, A_TRM, C_SET);
        printf("\nTrama SET enviada\n");

        alarmFlag = FALSE;
        alarm(TIMEOUT);

        int *size = malloc(sizeof(int));
        stateMachine(fd, A_TRM, C_UA, S, size);

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

int sendControlPackage(int fd, unsigned char *controlPackage, int *size, unsigned char bcc2, int s)
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

    write(fd, &buffer, bufferSize);

    return counter;
}

unsigned char *generateControlPackage(int fileSize, unsigned char *fileName, int *packageSize, int controlfield)
{
    int sizeFileName = strlen(fileName);
    int packSize = 9 * sizeof(unsigned char) + sizeFileName; //C,T1,L1,T2,L2(5) + sizeof(fileName) + tamanho campo filesize(4)
    unsigned char *controlPackage = (unsigned char *)(malloc(packSize));

    /* controlPackage = [C,T1,L1,V1,T2,L2,V2]
    * C = 2 (start) || C=3 (end)
    * T = 0 (tamanho ficheiro) || T = 1(nome ficheiro)
    * L = tamanho campo V
    * V = valor
    */
    controlPackage[0] = controlfield;
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

    //Cálculo nr tramas
    int nTramas;

    int l1 = size / 256;
    int l2 = size % 256;

    nTramas = l1;
    if (l2 != 0)
    {
        nTramas++;
    }

    unsigned char info[MAX_SIZE];

    info[0] = FLAG;
    info[1] = A_TRM;

    // for(int i=0; i<size; i++){
    //     printf("%x:", buffer[i]);
    //     if(size%256==0){
    //         printf("\n----------\n");
    //     }
    // }

    for (int i = 0; i < nTramas; i++)
    {

        int counter = 2;

        //send
        if (seqN == 0)
        {
            info[counter++] = 0x00;
        }
        else
        {
            info[counter++] = 0x40;
        }

        //BCC
        info[counter++] = info[1] ^ info[2];

        int *dataPackageSize = malloc(sizeof(int));
        *dataPackageSize = size;

        unsigned char *dataPackage = generateDataPackage(buffer, dataPackageSize, i, l1, l2);

        //BCC2
        unsigned char bcc2 = calculateBCC2(dataPackage, *dataPackageSize);

        //stuffing
        unsigned char *stuffedData = stuffingData(dataPackage, dataPackageSize);

        //data
        for (int i = 0; i < (*dataPackageSize); i++)
        {
            info[counter++] = stuffedData[i];
        }

        info[counter++] = bcc2;
        info[counter++] = FLAG;

        write(fd, &info, counter);

        (seqN == 0) ? seqN++ : seqN--;
    }
}

unsigned char *generateDataPackage(unsigned char *buffer, int *size, int n, int l1, int l2)
{
    unsigned char dataPackage[*size];

    dataPackage[0] = C_DATA;
    dataPackage[1] = n;
    dataPackage[2] = l1;
    dataPackage[3] = l2;

    int counter = 4;
    int dataSize = 4;
    //ultima trama

    if (n == l1)
    {
        for (int i = n * 256; i < n * 256 + l2; i++)
        {
            dataPackage[counter++] = buffer[i];
        }
        dataSize += l2;
    }
    else
    {
        for (int i = n * 256; i < n * 256 + 256; i++)
        {
            dataPackage[counter++] = buffer[i];
        }
        dataSize += 256;
    }

    int cnt = 0;
    unsigned char *dp = malloc((4 + dataSize) * sizeof(unsigned char));

    for (int i = 0; i < 4 + dataSize; i++)
    {
        dp[cnt++] = dataPackage[i];
    }

    *size = dataSize;

    return dp;
}

void closeConnection(int fd)
{
    do
    {
        sendControlMsg(fd, A_TRM, C_SET);
        printf("\nTrama DISC enviada\n");

        alarmFlag = FALSE;
        alarm(TIMEOUT);

        int *size = malloc(sizeof(int));
        stateMachine(fd, A_REC, C_DISC, S, size);
        printf("Trama DISC recebida!\n");

        sendControlMsg(fd, A_REC, C_UA);
        printf("Trama UA enviada!\n");
        printf("Connection closed!\n");

    } while (alarmFlag && numRetry < MAX_RETRY);
}