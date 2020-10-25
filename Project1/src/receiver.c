#include "receiver.h"

int receiveControlMsg(int fd, unsigned char control_field)
{
    Set_State_Machine state = START;
    unsigned char c;

    while (state != STOP)
    {
        read(fd, &c, 1); /* returns after 1 chars have been input */

        printf("STATE: %d\n", state);

        switch (state)
        {
        case START:
            if (c == FLAG)
                state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if (c == A)
                state = A_RCV;
            else if (c == FLAG)
            {
                state = FLAG_RCV;
            }
            else
                state = START;
            break;
        case A_RCV:
            if (c == control_field)
                state = C_RCV;
            else if (c == FLAG)
            {
                state = FLAG_RCV;
            }
            else
                state = START;
            break;
        case C_RCV:
            if (c == (A || control_field))
                state = BCC_OK;
            else if (c == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case BCC_OK:
            if (c == FLAG)
            {
                printf("TRAMA CORRETA\n");
                state = STOP;
            }
            else
                state = START;
            break;
        }
    }
    return TRUE;
}

void sendControlMsg(int fd, unsigned char control_field)
{
    unsigned char ua[5];
    ua[0] = FLAG;
    ua[1] = A;
    ua[2] = control_field;
    ua[3] = (A || control_field);
    ua[4] = FLAG;

    write(fd, ua, 5);
}

void LLOPEN(int fd)
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

    newtio.c_cc[VTIME] = 1; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 0;  /* blocking read until 5 chars received */

    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    if (receiveControlMsg(fd, C_SET))
    {
        printf("Trama SET recebida\n");
        sendControlMsg(fd, C_UA);
        printf("Trama UA enviada\n");
    }
}

int main(int argc, char **argv)
{
    int fd;

    if ((argc < 2) ||
        ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
         (strcmp("/dev/ttyS1", argv[1]) != 0)))
    {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
    fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(argv[1]);
        exit(-1);
    }

    LLOPEN(fd);

    close(fd);
    return 0;
}