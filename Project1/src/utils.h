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
#define C_DISC 0x0B

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

typedef enum
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} State_Machine;

typedef struct
{
    unsigned char *filename;
    int size;
} fileInfo;

/**
 * @brief Get file size
 * 
 * @param file file descriptor
 * @return file size
 */
int getFileSize(FILE *file);

/**
 * @brief Send Control Message
 * 
 * @param fd serial port descriptor
 * @param header header (A)
 * @param controlField control field (C)
 */
void sendControlMsg(int fd, unsigned char header, unsigned char controlField);

/**
 * @brief Message State Machine
 * 
 * @param fd serial port descriptor
 * @param header header (A)
 * @param controlField control field (C)
 * @param type I or S
 * @param size size of message
 * @return pointer to data from message
 */
unsigned char *stateMachine(int fd, unsigned char header, char controlField, int type, int *size);

/**
 * @brief Stuff Data
 * 
 * @param buffer buffer to be stuffed
 * @param size size of buffer
 * @return stuffed data 
 */
unsigned char *stuffingData(unsigned char *buffer, int *size);

/**
 * @brief Destuff Data
 * 
 * @param buffer buffer to be destuffed
 * @param size size of buffer
 * @return destuffed data
 */
unsigned char *destuffingData(unsigned char *buffer, int *size);

/**
 * @brief Calculate BCC2
 * 
 * @param buffer buffer to calculate bcc
 * @param size size of buffer
 * @return bcc2 value 
 */
unsigned char calculateBCC2(const unsigned char *buffer, unsigned int size);
