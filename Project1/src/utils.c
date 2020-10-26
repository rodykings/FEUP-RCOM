#include "utils.h"

int STOP_MACHINE = 0;

void sendControlMsg(int fd, unsigned char controlField)
{
    unsigned char msg[5];
    msg[0] = FLAG;
    msg[1] = A;
    msg[2] = controlField;
    msg[3] = (A || controlField);
    msg[4] = FLAG;
    write(fd, msg, 5);
}

void stateMachine(int *state, unsigned char c, char controlField)
{

    switch (*state)
    {
    case START:
        if (c == FLAG)
        {
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
        if (c == controlField)
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
        if (c == (A || controlField)) //BCC = A ^ C
            *state = BCC_OK;
        else
            *state = START;
        break;
    case BCC_OK:
        if (c == FLAG)
        {
            *state = STOP;
            STOP_MACHINE = 1;
        }
        else
            *state = START;
        break;
    }
}