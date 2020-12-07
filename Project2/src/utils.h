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

struct arguments {
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE]; 
    char filePath[MAX_SIZE];    
    char fileName[MAX_SIZE]; 
};

int verify_ftp(const char *str);
int parseURL(char *complete_url, struct arguments *args);
char *get_ip_address(char *host_name);