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

    newtio.c_cc[VTIME] = 1; /* inter-unsigned character timer unused */
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
    if (dataInfo.size % 256 != 0)
        nTramas++;

    int *size = malloc(sizeof(int));

    unsigned char *fileData = malloc(sizeof(unsigned char) * dataInfo.size);

    int counter = 0;
    int n = -1;
    int currentN = 0;
    for (int i = 0; i < nTramas; i++)
    {

        unsigned char *data = stateMachine(fd, A_TRM, 0x00, I, size);
        currentN = data[1];

        if (currentN == n+1)
        {
            printf("%d\n", data[1]);
            for (int d = 4; d < (*size) - 1; d++)
            {
                fileData[counter++] = data[d];
            }
            n=currentN;

        }else if(data == NULL){
            i--;
        }else{
            i--;
        }
    }

    printf("SIZE: %x", counter);

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