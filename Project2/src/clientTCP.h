#include "utils.h"

int open_socket(int port, char *address);
int write_to_socket(int sockfd, char* buf, size_t size);
int read_from_socket(int sockfd, char *buf, size_t buf_size);
int login(int sockfd, char*user, char*password);
int change_directory(int sockfd, char *path);
int passive_mode(int sockfd);
int retrieve(int sockfd, char*file);
int download(int sockfd, char* file);
int close_socket(int sockfd);
