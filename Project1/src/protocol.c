#include "protocol.h"

int llopen(int fd, int status)
{
    if (tcgetattr(fd, &oldtio) == -1)
    { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 3; /* inter-unsigned character timer unused */
    newtio.c_cc[VMIN] = 0;  /* blocking read until 5 unsigned chars received */

    /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) proximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    printf("\nStarting connection...\n\n");

    if (status == TRANSMITTER)
    {

        if (!setTransmitter(fd))
        {
            printf("\nCommunication protocol failed after %d tries!\n\n", MAX_RETRY);
            return -1;
        }
    }
    else if (status == RECEIVER)
    {
        setReceiver(fd);
    }
    else
    {
        printf("Invalid status!\n");
        return -1;
    }
    return 0;
}

int llwrite(int fd, FILE *file, char *filename)
{
    int fileSize = getFileSize(file);

    unsigned char *buffer = malloc(sizeof(unsigned char) * fileSize);
    /*Lê ficheiro*/
    fread(buffer, sizeof(unsigned char), fileSize, file);

    int seqN = sendControl(fd, fileSize, filename, 0x02);

    sendData(fd, buffer, fileSize, seqN);

    sendControl(fd, fileSize, filename, 0x03);

    return 0;
}

int llread(int fd)
{
    fileInfo dataInfo = receiveControlPackage(fd);

    int nTramas = dataInfo.size / 256;
    
    int l1 = dataInfo.size / 256;
    int l2 = dataInfo.size % 256;

    if(l2!=0){
        nTramas++;
    }

    printf("NTRMAS: %d", nTramas);
    int *size = malloc(sizeof(int));

    unsigned char *fileData = malloc(sizeof(unsigned char) * dataInfo.size);

    int counter = 0;
    int n = -1;
    int currentN = 0;
    int fail = FALSE;
    for (int i = 0; i < nTramas; i++)
    {

        if(fail == TRUE){
            printf("NTramas: %d\n falhou aqui - %d\n", nTramas, i);
        }

        unsigned char *data = stateMachine(fd, A_TRM, 0x00, I, size);
        currentN = data[1];

        if (currentN == n + 1)
        {
            if (i < nTramas - 1 && (*size >= 255 && *size <= 261))
            {
                int cnt = 0;
                for (int d = 4; d < (*size) - 1; d++)
                {
                    cnt++;
                    fileData[counter++] = data[d];
                }
                printf("TRAMA - %d | SIZE: %d\n", data[1], cnt);
                fail = FALSE;
                n = currentN;
            }
            else if (i == nTramas - 1 && (*size >= l2 && *size <= l2+5))
            {
                int cnt = 0;
                for (int d = 4; d < (*size) - 1; d++)
                {
                    cnt++;
                    fileData[counter++] = data[d];
                }
                printf("TRAMA ULTIMA - %d | SIZE: %d\n", data[1], cnt);
                n = currentN;
            }
            else if(*size>261){
                int cnt = 0;
                for (int d = (*size)-257; d < (*size) - 1; d++)
                {
                    cnt++;
                    fileData[counter++] = data[d];
                }
                printf("TRAMA - %d | SIZE: %d\n", data[1], cnt);
                fail = FALSE;
                n = currentN;
            }
            else
            {
                printf("%d\n", *size);
                printf("trama incorreta com i = %d\n", i);
                sendControlMsg(fd, A_TRM, 0x01);
                i--;
                printf("Diminui i: %d\n", i);
            }
        }else if(currentN == n){//duplicado
            printf("RECEBI DUPLICADO\n");
            i--;
        }
        else
        {
            printf("FALHOU CURRENTN - %d - I:  %d\n", currentN, i);
            i--;

        }

        if(currentN == 255){
            n = -1;
        }
    }

    printf("SIZE: %d", counter);

    fileInfo dataInfoFinal = receiveControlPackage(fd);

    createFile(dataInfo, fileData);

    return 0;
}

int llclose(int fd, int status)
{
    if (status == TRANSMITTER)
    {
        closeConnection(fd);
    }
    if (status == RECEIVER)
    {
        handleDisconnection(fd);
    }
    close(fd);

    return 0;
}