#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

//Stuffing macros
#define ESCAPEMENT 0x7D
#define REPLACE_FLAG 0x5E
#define REPLACE_ESCAPEMENT 0x5D

#define MAX_RETRY 5
#define MAX_SIZE 256

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

struct termios oldtio, newtio;

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} State_Machine;

void sendControlMsg(int fd, unsigned char controlField);
void stateMachine(int *state, unsigned char c, char controlField);
char* stuffingData(char* buffer, int size);
void unstuffingData(char* buffer, int size);
unsigned char calculateBCC2(const unsigned char *buffer, unsigned int size);
