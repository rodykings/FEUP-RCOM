#pragma once
#include "utils.h"

void setReceiver(int fd);
int receiveControlMsg(int fd, unsigned char control_field);
fileInfo receiveControlPackage(int fd);
int checkControlPackage(unsigned char*controlPackage, int*size, fileInfo* fileinfo);
void createFile(fileInfo info, unsigned char* fileData);
void handleDisconnection(int fd);