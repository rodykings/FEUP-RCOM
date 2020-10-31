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

    if (status == TRANSMITTER)
    {

        if (!setTransmitter(fd))
        {
            printf("\nCommunication protocol failed after %d tries!", MAX_RETRY);
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

int llwrite(int fd, unsigned char *filename)
{
    int writtenCharacters = 0;

    int file = open(filename, O_RDONLY);

    int fileSize = lseek(file, 0, SEEK_END);

    char buffer[fileSize];

    //Cálculo nr tramas
    int nTramas = fileSize / 256;
    if (fileSize % 256 != 0)
    {
        nTramas++;
    }

    //Lê informação do ficheiro até ao fim
    for (int i = 0; i < fileSize; i++)
    {
        read(file, &buffer, fileSize);
    }

    //Envia trama de controlo

    int* size = malloc(sizeof(int));
    unsigned char *controlPackage = generateControlPackage(fileSize, filename, size);

    //Calculo do BCC com informacao
    unsigned char bcc2 = calculateBCC2(controlPackage, size);

    unsigned char *stuffedControlPackage = stuffingData(controlPackage, size);    

    writtenCharacters += sendControlPackage(fd, stuffedControlPackage, size, bcc2, 0);
    printf("\nControl Packaged Sent!\n");
    //Espera pelo Aknowledge - máquina de estados

    //Stuffing
    //stuffingData(buffer, sizeStuffedBuffer);

    //sendData package cicle;

    printf("Nr caracteres escritos: %d\n", writtenCharacters);
    return writtenCharacters;
}

int llread(int fd, unsigned char *buffer)
{
    receiveControlPackage(fd); //TODO -> como passar o size que tem de ler na trama de controlo?
    return 0;
}

void llclose(int fd)
{

    //all ending connections

    close(fd);
}