#pragma once
#include "utils.h"
#include "receiver.h"
#include "transmitter.h"

#define TRANSMITTER 0
#define RECEIVER 1

/**
 * @brief 
 * 
 * @param fd serial port descriptor 
 * @param status TRANSMITTER or RECEIVER
 * @return execution status
 */
int llopen(int fd, int status);


/**
 * @brief Write to Serial Port
 * 
 * @param fd serial port descriptor 
 * @param file file pointer
 * @param filename filename
 * @return execution status
 */
int llwrite(int fd, FILE* file, char* filename);


/**
 * @brief Read from Serial Port
 * 
 * @param fd serial port descriptor
 * @return execution status
 */
int llread(int fd);


/**
 * @brief Close connection
 * 
 * @param fd serial port descriptor
 * @param status TRANSMITTER or RECEIVER
 * @return execution status
 */
int llclose(int fd, int status);
