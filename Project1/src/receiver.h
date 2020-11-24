#pragma once
#include "utils.h"

/**
 * @brief Set receiver connection
 * 
 * @param fd serial port descriptor
 */
void setReceiver(int fd);

/**
 * @brief 
 * 
 * @param fd serial port descriptor
 * @return file information
 */
fileInfo receiveControlPackage(int fd);

/**
 * @brief Verify control package
 * 
 * @param controlPackage control package
 * @param size size of control package
 * @param fileinfo file information
 * @return control bit (2 or 3)
 */
int checkControlPackage(unsigned char *controlPackage, int *size, fileInfo *fileinfo);

/**
 * @brief Create a file and write
 * 
 * @param info file info
 * @param fileData buffer with file data
 */
void createFile(fileInfo info, unsigned char *fileData);

/**
 * @brief Disconnect receiver
 * 
 * @param fd serial port descriptor
 */
void handleDisconnection(int fd);