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

void sendControlPackage(int fd, unsigned char *controlPackage, int *size, unsigned char bcc2, int s)
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

    buffer[counter++] = FLAG;

    write(fd, &buffer, bufferSize);
}

unsigned char *generateControlPackage(int fileSize, unsigned char *fileName, int *packageSize, int controlfield)
{
    int sizeFileName = strlen(fileName);
    int packSize = 9 * sizeof(unsigned char) + sizeFileName; //C,T1,L1,T2,L2(5) + sizeof(fileName) + tamanho campo filesize(4)
    unsigned char *controlPackage = malloc(sizeof(unsigned char) * (packSize + 1));

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

    *packageSize = packSize + 1;

    return controlPackage;
}

void sendData(int fd, unsigned char *buffer, int size, int seqN)
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
        unsigned char bcc2 = calculateBCC2(dataPackage, *dataPackageSize - 1);

        dataPackage[*dataPackageSize - 1] = bcc2;

        //stuffing
        unsigned char *stuffedData = stuffingData(dataPackage, dataPackageSize);

        //data
        for (int i = 0; i < (*dataPackageSize); i++)
        {
            info[counter++] = stuffedData[i];
        }

        info[counter++] = FLAG;

        do
        {
            write(fd, &info, counter);

            alarmFlag = FALSE;
            alarm(TIMEOUT);

            int *size = malloc(sizeof(int));

            int c_state;

            if (seqN == 0)
            {
                c_state = 0x05; //Expects positive ACK -> controlField val = 0x05 (R = 0)
            }
            else
            {
                c_state = 0x85; //Expects positive ACK -> controlField val = 0x85 (R = 1)
            }

            unsigned char *status = stateMachine(fd, A_TRM, c_state, S, size);
            printf("Status: %x\n", status[0]);
            if (status[0] == 0x0)
            {
                printf("Trama RR recebida!\n");
                break;
            }
            else if (status[0] == 0x1)
            {
                printf("Trama RJ recebida - send Data!\n");
            }
            else
            {
                printf("Waiting.... \n");
                for (int i = 0; i < counter; i++)
                {
                    printf("%x:", info[i]);
                }
            }

            (seqN == 0) ? seqN++ : seqN--;

        } while (alarmFlag && numRetry < MAX_RETRY);

        if (alarmFlag && numRetry == MAX_RETRY)
            return;

        (seqN == 0) ? seqN++ : seqN--;
    }
}

int sendControl(int fd, int fileSize, unsigned char *fileName, int controlField)
{
    //Envia trama de controlo
    int *size = malloc(sizeof(int));
    unsigned char *controlPackage = generateControlPackage(fileSize, fileName, size, controlField);

    //Calculo do BCC com informacao
    unsigned char bcc2 = calculateBCC2(controlPackage, *size - 1);

    controlPackage[*size - 1] = bcc2;

    unsigned char *stuffedControlPackage = stuffingData(controlPackage, size);

    //Espera pelo Aknowledge - máquina de estados
    int seqN = 0;
    do
    {
        sendControlPackage(fd, stuffedControlPackage, size, bcc2, seqN);

        alarmFlag = FALSE;
        alarm(TIMEOUT);

        int *size = malloc(sizeof(int));

        int c_state;

        if (seqN == 0)
        {
            c_state = 0x05; //Expects positive ACK -> controlField val = 0x05 (R = 0)
        }
        else
        {
            c_state = 0x85; //Expects positive ACK -> controlField val = 0x85 (R = 1)
        }

        unsigned char *status = stateMachine(fd, A_TRM, c_state, S, size);
        if (status[0] == 0x0)
        {
            printf("Trama RR recebida!\n");
            break;
        }
        else if(status[0] == 0x1) 
        {
            printf("Trama RJ recebida - send Control!\n");
        }

        (seqN == 0) ? seqN++ : seqN--;

    } while (alarmFlag && numRetry < MAX_RETRY);
    printf("\nTrama I de controlo enviada!\n");

    return seqN;
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

    *size = dataSize + 1;

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