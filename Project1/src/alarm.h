#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_RETRY 4

void alarmHandler(int signal);
