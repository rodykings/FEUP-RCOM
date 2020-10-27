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
void sendControlPackage(int fd, char* controlPackage, int size, int s);
char* generateControlPackage(int fileSize, char *fileName);
char* generateDataPackage(int n, int l1, int l2, char* buffer);