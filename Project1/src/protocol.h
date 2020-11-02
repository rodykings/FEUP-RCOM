#pragma once
#include "utils.h"
#include "receiver.h"
#include "transmitter.h"

#define TRANSMITTER 0
#define RECEIVER 1

int llopen(int fd, int status);


/**
 * @brief 
 * 
 * @param fd identificador da ligação de dados
 * @param buffer array de caracteres a transmitir
 * @param length comprimento do array de caracteres
 * @return int número de caracteres escritos
 */
int llwrite(int fd, unsigned char * filename);

/**
 * @brief 
 * 
 * @param fd identificador da ligação de dados
 * @param buffer array de caracteres recebidos
 * @return int comprimento do array (número de caracteres lidos)
 */
int llread(int fd);

void llclose(int fd, int status);
