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

    //Abre o ficheiro
    FILE *file = fopen(filename, "rb");
    int fileSize = getFileSize(file);

    unsigned char *buffer = malloc(sizeof(unsigned char) * fileSize);
    /*Lê ficheiro*/
    fread(buffer, sizeof(unsigned char), fileSize, file);


    //PRINT DO BUFFER
    /*
    for (int i = 0; i < fileSize; i++)
    {
        if (i % 256 == 0)
        {
            printf("\n----BLOCO %d------\n", i / 256);
        }
        printf("%x:", buffer[i]);
    }*/

    //Envia trama de controlo
    int *size = malloc(sizeof(int));
    unsigned char *controlPackage = generateControlPackage(fileSize, filename, size, 0x02);

    


    //Calculo do BCC com informacao
    unsigned char bcc2 = calculateBCC2(controlPackage, *size-1);

    controlPackage[*size-1] = bcc2;

    

    unsigned char *stuffedControlPackage = stuffingData(controlPackage, size);
   

    //Espera pelo Aknowledge - máquina de estados
    int seqN = 0;
    do
    {
        writtenCharacters = 0;
        writtenCharacters += sendControlPackage(fd, stuffedControlPackage, size, bcc2, seqN);

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
        if (status[0] == 'A')
        {
            printf("Trama RR recebida!\n");
            break;
        }
        else
        {
            printf("Trama RJ recebida!\n");
        }

        (seqN == 0) ? seqN++ : seqN--;

    } while (alarmFlag && numRetry < MAX_RETRY);
    printf("\nTrama I de controlo enviada!\n");

    /*
    for(int i=0; i<fileSize; i++){
        printf("%x:", buffer[i]);
    }*/
    sendData(fd, buffer, fileSize, seqN);

    //Envia trama de controlo
    int *finalSize = malloc(sizeof(int));
    unsigned char *finalControlPackage = generateControlPackage(fileSize, filename, finalSize, 0x03);

    //Calculo do BCC com informacao
    unsigned char finalBcc2 = calculateBCC2(finalControlPackage, *finalSize-1);

    controlPackage[*size-1] = bcc2;

    unsigned char *stuffedFinalControlPackage = stuffingData(finalControlPackage, finalSize);



    //Espera pelo Aknowledge - máquina de estados
    int finalSeqN = 0;
    do
    {
        writtenCharacters = 0;
        writtenCharacters += sendControlPackage(fd, stuffedFinalControlPackage, finalSize, finalBcc2, finalSeqN);

        alarmFlag = FALSE;
        alarm(TIMEOUT);

        int *size = malloc(sizeof(int));

        int c_state;

        if (finalSeqN == 0)
        {
            c_state = 0x05; //Expects positive ACK -> controlField val = 0x05 (R = 0)
        }
        else
        {
            c_state = 0x85; //Expects positive ACK -> controlField val = 0x85 (R = 1)
        }

        unsigned char *status = stateMachine(fd, A_TRM, c_state, S, size);
        if (status[0] == 'A')
        {
            printf("Trama RR recebida!\n");
            break;
        }
        else
        {
            printf("Trama RJ recebida!\n");
        }

        (finalSeqN == 0) ? finalSeqN++ : finalSeqN--;

    } while (alarmFlag && numRetry < MAX_RETRY);
    printf("\nTrama I de controlo final enviada!\n");

    return writtenCharacters;
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
    int cnt = 0;
    for (int i = 0; i < nTramas; i++)
    {
        cnt = 0;

        printf("\nTRAMA %d-------------\n", i);
        unsigned char *data = stateMachine(fd, A_TRM, 0x00, I, size);

        // printf("SIZE: %d\n", *size);
        for (int d = 4; d < (*size) - 1; d++)
        {
            fileData[counter++] = data[d];
           /* if (i == 1)
            {
                printf("%d:", cnt);
                printf("%x\n", fileData[counter - 1]);
            }*/
            //
            cnt++;
        }

        if (data == NULL)
        {
            printf("RECEBEU NULL: %d", i);
            i--;
        }
        //printf("\n--------------\n");
        //printf("\nSENT: %d bytes - %d/%d\n", cnt, counter, dataInfo.size);
    }
    // printf("\nCOUNTER: %d", counter);

    fileInfo dataInfoFinal = receiveControlPackage(fd);

    createFile(dataInfo, fileData);

    return 0;
}

void llclose(int fd, int status)
{
    if(status == TRANSMITTER){
        closeConnection(fd);
    }
    if(status == RECEIVER){
        handleDisconnection(fd);
    }

    //all ending connections

    close(fd);
}