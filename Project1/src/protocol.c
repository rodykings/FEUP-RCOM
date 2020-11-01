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

int llwrite(int fd, unsigned char *filename)
{
    int writtenCharacters = 0;

    int file = open(filename, O_RDONLY);

    int fileSize = lseek(file, 0, SEEK_END);

    char buffer[fileSize];
    
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

    
    //Espera pelo Aknowledge - máquina de estados
    int seqN = 0;
    do
    {
        writtenCharacters = 0;
        writtenCharacters += sendControlPackage(fd, stuffedControlPackage, size, bcc2, seqN);
        
        alarmFlag = FALSE;
        alarm(TIMEOUT);

        int * size = malloc(sizeof(int));
        
        int c_state;

        if(seqN == 0){
            c_state = 0x05; //Expects positive ACK -> controlField val = 0x05 (R = 0)
        }else {
            c_state = 0x85; //Expects positive ACK -> controlField val = 0x85 (R = 1)
        }
        
        unsigned char* status = stateMachine(fd, c_state, S, size);
        if(status[0] == 'A'){
            printf("Trama RR recebida!\n");
            break;
        }
        else{
            printf("Trama RJ recebida!\n");
        }
        
        (seqN == 0) ? seqN++: seqN--;

    } while (alarmFlag && numRetry < MAX_RETRY);
    printf("\nTrama I de controlo enviada!\n");

/*
    for(int i=0; i<fileSize; i++){
        printf("%x:", buffer[i]);
    }*/
    sendData(fd, buffer, fileSize,seqN);



    // printf("Nr caracteres escritos: %d\n", writtenCharacters);
    return writtenCharacters;
}

int llread(int fd)
{
    receiveControlPackage(fd);
    int sizeT = 10968 + 6*43 + 4*43;
    unsigned char buffer[1];
    for(int i=0; i<sizeT; i++){
        read(fd, &buffer, 1);
        printf("%x:",buffer[1]);
    }

    return 0;
}

void llclose(int fd)
{

    //all ending connections

    close(fd);
}