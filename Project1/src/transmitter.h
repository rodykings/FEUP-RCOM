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

int setTransmitter(int fd);
int sendControlPackage(int fd, unsigned char *controlPackage, int *size, unsigned char bcc2, int s);
unsigned char *generateControlPackage(int fileSize, unsigned char *fileName, int *packageSize, int controlfield);
unsigned char *sendData(int fd, unsigned char *buffer, int size, int seqN);
unsigned char *generateDataPackage(unsigned char *buffer, int *size, int n, int l1, int l2);
int sendControl(int fd, int fileSize, unsigned char *fileName, int controlField);
void closeConnection(int fd);