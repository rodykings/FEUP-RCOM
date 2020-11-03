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

/**
 * @brief Set transmitter connection
 * 
 * @param fd serial port descriptor
 * @return execution status
 */
int setTransmitter(int fd);


/**
 * @brief Send a control package
 * 
 * @param fd serial port descriptor
 * @param controlPackage control package buffer
 * @param size size of control package
 * @param bcc2 bcc2 value
 * @param s message type 
 */
void sendControlPackage(int fd, unsigned char *controlPackage, int *size, unsigned char bcc2, int s);

/**
 * @brief Generate a control package
 * 
 * @param fileSize file size
 * @param fileName file name
 * @param packageSize package size
 * @param controlfield control field (0x02 or 0x03)
 * @return pointer to control package
 */
unsigned char *generateControlPackage(int fileSize, unsigned char *fileName, int *packageSize, int controlfield);

/**
 * @brief Send all data
 * 
 * @param fd serial port descriptor
 * @param buffer data buffer
 * @param size size of data
 * @param seqN sequence number
 */
void sendData(int fd, unsigned char *buffer, int size, int seqN);

/**
 * @brief Generate a data package
 * 
 * @param buffer file buffer
 * @param size file size
 * @param n data sequence number
 * @param l1 number of complete sequences
 * @param l2 number of bytes in last sequence
 * @return pointer to data package
 */
unsigned char *generateDataPackage(unsigned char *buffer, int *size, int n, int l1, int l2);

/**
 * @brief Send Control Message
 * 
 * @param fd serial port descriptor
 * @param fileSize file size
 * @param fileName file name
 * @param controlField control field
 * @return sequence number 
 */
int sendControl(int fd, int fileSize, unsigned char *fileName, int controlField);

/**
 * @brief Close transmitter connection
 * 
 * @param fd serial port descriptor
 */
void closeConnection(int fd);