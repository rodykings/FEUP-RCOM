#include "alarm.h"

void alarmHandler()
{
    printf("\nAlarm: %d\n", numRetry + 1);
    alarmFlag = TRUE;
    numRetry++;
}
