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
char *get_ip_address(char *host_name);
int parse_passive_response(char *response, char *ip_address, int *port);