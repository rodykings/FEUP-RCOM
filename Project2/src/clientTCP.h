#include "utils.h"

int open_socket(int port, char *address);
int write_to_socket(int sockfd, char* buf, size_t size);
int read_from_socket(int sockfd, char *buf, size_t buf_size);
int login(int sockfd, char*user, char*password);
int close_socket(int sockfd);
