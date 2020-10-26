#pragma once

#include "utils.h"
#include "alarm.h"

#define TIMEOUT 5

extern int numRetry;
extern int alarmFlag;

int setTransmitter(int fd);