#pragma once

#include <fcntl.h>
#include "utils.h"
#include "alarm.h"

#define TIMEOUT 5

#define C_START 0x02
#define C_DATA 0x01
#define T1 0x00
#define T2 0x01 
#define L1 0x04

extern int numRetry;
extern int alarmFlag;

/**
 * @brief Send SET message to the receiver and
 * receive and process UA message to establish connection with receiver
 * 
 * @param fd File descriptor
 * @return int 1 to success and 0 to failure
 */
int setTransmitter(int fd);

/**
 * @brief Sends message with control information - control package
 * 
 * @param fd File descriptor
 * @param controlPackage Control Package
 * @param size Size of control package with stuffed data
 * @param bcc2 Value of BCC2
 * @param s Value of control field - alternates between 0 and 1
 * @return int Size of control package
 */
int sendControlPackage(int fd, unsigned char *controlPackage, int* size, unsigned char bcc2, int s);
unsigned char *generateControlPackage(int fileSize, unsigned char *fileName, int*packageSize, int controlfield);
unsigned char *sendData(int fd, unsigned char *buffer, int size, int seqN);
unsigned char * generateDataPackage(unsigned char *buffer, int* size, int n, int l1, int l2);