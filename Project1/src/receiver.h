#pragma once
#include "utils.h"

void setReceiver(int fd);
int receiveControlMsg(int fd, unsigned char control_field);
