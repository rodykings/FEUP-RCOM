#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>
#include <errno.h>

#define MAX_SIZE 256

/**
 * @brief Struct to store information about elements received in the URL
 * 
 */
struct arguments
{
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE];
    char filePath[MAX_SIZE];
    char fileName[MAX_SIZE];
};

/**
 * @brief Struct to store sockets file descriptors
 * 
 */
struct ftp
{
    int sockfd;
    int datafd;
};

/**
 * @brief Verify if a string matches ftp://
 * 
 * @param str String to compare
 * @return int 0 if str is not equal to ftp://
 */
int verify_ftp(const char *str);

/**
 * @brief Parse all elements of the received URL and fills the args struct with parsed information
 * 
 * @param complete_url URL to parse
 * @param args Struct to store information about all the elements received in the URL
 * @return int 0 in case of sucessful parse or -1 if any error occurs
 */
int parseURL(char *complete_url, struct arguments *args);

/**
 * @brief Print on the screen information stored on args
 * 
 * @param args Struct with information
 */
void printInfo(struct arguments *args);

/**
 * @brief Get the ip address of host
 * 
 * @param host_name Hostname
 * @return char* IP address
 */
char *get_ip_address(char *host_name);

/**
 * @brief Send command to server in order to enter on passive mode
 * 
 * @param response Server response
 * @param ip_address New ip address for passive mode
 * @param port New port to open socket on passive mode
 * @return int 0 in case of success or -1 if any error occurs
 */
int parse_passive_response(char *response, char *ip_address, int *port);