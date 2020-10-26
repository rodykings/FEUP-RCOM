#pragma once
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

extern int numRetry;
extern int alarmFlag;

void alarmHandler();
