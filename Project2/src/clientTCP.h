#include "utils.h"

int open_socket(int port, char* address);
int write_to_socket(int sockfd);
int read_from_socket(int sockfd);
int close_socket(int sockfd);
