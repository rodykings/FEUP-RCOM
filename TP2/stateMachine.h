#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define C 0x03

volatile int STOP=FALSE;

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP_UA} UA_State_Machine;

int stateMachine(int res, int fd, char buf[]);