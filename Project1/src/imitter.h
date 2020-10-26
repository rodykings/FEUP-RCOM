#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define MAX_RETRY 5
#define TIMEOUT 5

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

struct termios oldtio, newtio;

typedef enum
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} UA_State_Machine;

void sendControlMsg(int fd, unsigned char control_field);
void stateMachineUA(UA_State_Machine state, unsigned char c);
int LLOPEN(int fd);
void alarmHandler();
int main(int argc, char **argv);