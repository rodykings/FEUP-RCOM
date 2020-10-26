#include "imitter.h"

int num_retry = 0;
int alarm_flag = FALSE;
int STOP_UA_MACHINE = FALSE;

void sendControlMsg(int fd, unsigned char control_field)
{
    unsigned char set[5];
    set[0] = FLAG;
    set[1] = A;
    set[2] = control_field;
    set[3] = (A || control_field);
    set[4] = FLAG;
    write(fd, set, 5);
}

void stateMachineUA(UA_State_Machine *state, unsigned char c)
{

    switch (*state)
    {
    case START:
        if (c == FLAG){
            *state = FLAG_RCV;
        }
        break;
    case FLAG_RCV:
        if (c == A)
            *state = A_RCV;
        else
        {
            if (c == FLAG)
                *state = FLAG_RCV;
            else
                *state = START;
        }
        break;
    case A_RCV:
        if (c == C_UA)
           *state = C_RCV;
        else
        {
            if (c == FLAG)
                *state = FLAG_RCV;
            else
                *state = START;
        }
        break;
    case C_RCV:
        if (c == (A || C_UA)) //BCC = A ^ C
            *state = BCC_OK;
        else
            *state = START;
        break;
    case BCC_OK:
        if (c == FLAG)
        {
            *state = STOP;
            STOP_UA_MACHINE = TRUE;
            printf("Recebeu trama UA\n");
        }
        else
            *state = START;
        break;
    }
}

int LLOPEN(int fd)
{
    unsigned char c;

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

    do
    {
        sendControlMsg(fd, C_SET);
        printf("Trama SET enviada\n");

        alarm_flag = FALSE;
        alarm(TIMEOUT);

        int state = START;
        while (!STOP_UA_MACHINE && !alarm_flag)
        {
            read(fd, &c, 1);
            stateMachineUA(&state, c);
        }
    } while (alarm_flag && num_retry < MAX_RETRY);

    //Comunicação falha após atingir o número máximo de tentativas
    if (alarm_flag && num_retry == MAX_RETRY)
        return FALSE;

    else
    {
        alarm_flag = FALSE;
        num_retry = 0;
        printf("\nProtocol connection established!");
        return TRUE;
    }
}

void alarmHandler()
{
    printf("\nAlarm: %d", num_retry + 1);
    alarm_flag = TRUE;
    num_retry++;
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

    // Register signal handler
    //Enquanto não exceder o número de tentativas enviar até receber resposta
    (void)signal(SIGALRM, alarmHandler);

    if (!LLOPEN(fd)){
        printf("\nCommunication protocol failed after %d tries!", MAX_RETRY);
        return -1;
    }
        

    close(fd);
    return 0;
}