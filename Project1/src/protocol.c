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

int llwrite(int fd, char *filename)
{
    int writtenCharacters = 0;

    int file = open(filename, O_RDONLY);

    int size = lseek(file, 0, SEEK_END);

    char buffer[size];

    //Cálculo nr tramas
    int nTramas = size / 256;
    if (size % 256 != 0)
    {
        nTramas++;
    }

    //Lê informação do ficheiro até ao fim
    for (int i = 0; i < size; i++)
    {
        read(file, &buffer, size);
    }

    //Envia trama de controlo
    char *controlPackage = generateControlPackage(size, filename);
    int sizeControlPackage = 5 + sizeof(size) + sizeof(filename);

    //Calculo do BCC com informacao
    unsigned char bcc2 = calculateBCC2(controlPackage, sizeControlPackage);

    //stuffing process
    int sizeStuffedBuffer = calculateSize(controlPackage, sizeControlPackage);
    char *stuffedControlPackage = stuffingData(controlPackage, sizeStuffedBuffer);

    writtenCharacters += sendControlPackage(fd, stuffedControlPackage, sizeStuffedBuffer, bcc2, 0);

    //Espera pelo Aknowledge - máquina de estados

    //Stuffing
    //stuffingData(buffer, sizeStuffedBuffer);

    //sendData package cicle;

    printf("Nr caracteres escritos: %d\n", writtenCharacters);
    return writtenCharacters;
}

int llread(int fd, char *buffer)
{
    receiveControlPackage(fd, 23); //TODO -> como passar o size que tem de ler na trama de controlo?
    return 0;
}

void llclose(int fd)
{

    //all ending connections

    close(fd);
}