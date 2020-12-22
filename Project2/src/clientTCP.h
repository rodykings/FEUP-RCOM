#include "utils.h"

/**
 * @brief Open new socket using specified port and IP address 
 * 
 * @param port Port
 * @param address IP address
 * @return int 0 in case of success or -1 if any error occurs
 */
int open_socket(int port, char *address);

/**
 * @brief Send a message to the socket with specified file descriptor 
 * 
 * @param fd Socket file descriptor
 * @param buf Message to send
 * @param size Size of message
 * @return int 0 in case of success or -1 if any error occurs
 */
int write_to_socket(int fd, char *buf, size_t size);

/**
 * @brief Read from socket with specified file descriptor
 * 
 * @param fd Socket file descriptor
 * @param buf Message to be read
 * @param buf_size Size of message
 * @return int 0 in case of success or -1 if any error occurs
 */
int read_from_socket(int fd, char *buf, size_t buf_size);

/**
 * @brief Login into the FTP server with received credentials
 * 
 * @param ftp_connnection Struct with sockets file descriptors
 * @param user User 
 * @param password User's password
 * @return int 0 in case of success or -1 if any error occurs
 */
int login(struct ftp *ftp_connnection, char *user, char *password);

/**
 * @brief Change server directory
 * 
 * @param ftp_connection Struct with sockets file descriptors
 * @param path New path
 * @return int 0 in case of success or -1 if any error occurs
 */
int change_directory(struct ftp *ftp_connection, char *path);

/**
 * @brief Enter in passive mode
 * 
 * @param ftp_connection Struct with sockets file descriptors
 * @return int 0 in case of success or -1 if any error occurs
 */
int passive_mode(struct ftp *ftp_connection);

/**
 * @brief File starts being transmited in the file socket
 * 
 * @param ftp_connection Struct with sockets file descriptors
 * @param file File name
 * @return int 0 in case of success or -1 if any error occurs
 */
int retrieve(struct ftp *ftp_connection, char *file);

/**
 * @brief Download file from FTP server
 * 
 * @param ftp_connection Struct with sockets file descriptors
 * @param file Transferred file name
 * @return int 0 in case of success or -1 if any error occurs
 */
int download(struct ftp *ftp_connection, char *file);

/**
 * @brief Close connection and sockets
 * 
 * @param ftp_connection Struct with sockets file descriptors
 * @return int 0 in case of success or -1 if any error occurs
 */
int close_socket(struct ftp *ftp_connection);
