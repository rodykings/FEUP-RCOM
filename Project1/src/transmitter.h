#pragma once

#include <fcntl.h>
#include "utils.h"
#include "alarm.h"

#define TIMEOUT 5

#define C_START 0x2;
#define T1 0x0;
#define T2 0x1; 

extern int numRetry;
extern int alarmFlag;

int setTransmitter(int fd);
int sendControlPackage(int fd, int fileSize, char* fileName);