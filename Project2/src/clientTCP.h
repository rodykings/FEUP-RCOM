#include "utils.h"

int open_socket(int port, char *address);
int write_to_socket(int fd, char *buf, size_t size);
int read_from_socket(int fd, char *buf, size_t buf_size);
int login(struct ftp *ftp_connnection, char *user, char *password);
int change_directory(struct ftp *ftp_connection, char *path);
int passive_mode(struct ftp *ftp_connection);
int retrieve(struct ftp *ftp_connection, char *file);
int download(struct ftp *ftp_connection, char *file);
int close_socket(struct ftp *ftp_connection);
