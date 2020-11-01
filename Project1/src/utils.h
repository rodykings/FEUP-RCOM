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

#define I 1 
#define S 0


#define FLAG 0x7E
#define A_TRM 0x03
#define A_REC 0x01
#define C_SET 0x03
#define C_UA 0x07

//Stuffing macros
#define ESCAPEMENT 0x7D
#define REPLACE_FLAG 0x5E
#define REPLACE_ESCAPEMENT 0x5D

#define MAX_RETRY 5
#define MAX_SIZE 1024

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define NULL ((void *)0)

struct termios oldtio, newtio;

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} State_Machine;


typedef struct {
    unsigned char* filename;
    int size;
}fileInfo;

int getFileSize(FILE* file);
void sendControlMsg(int fd, unsigned char controlField);
unsigned char* stateMachine(int fd, char controlField, int type, int* size);
unsigned char* stuffingData(char* buffer, int* size);
unsigned char* destuffingData(char *buffer, int *size);
int calculateStuffedSize(char* buffer, int size);
unsigned char calculateBCC2(const unsigned char *buffer, unsigned int size);
