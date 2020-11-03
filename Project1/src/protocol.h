#pragma once
#include "utils.h"
#include "receiver.h"
#include "transmitter.h"

#define TRANSMITTER 0
#define RECEIVER 1

int llopen(int fd, int status);

void llwrite(int fd, FILE* file, char* filename);

int llread(int fd);

void llclose(int fd, int status);
