#include "utils.h"
#include "clientTCP.h"

//Example: ./download ftp://ftp.up.pt/pub/file.md

int main(int argc, char *argv[])
{
    char buffer[MAX_SIZE];

    if (argc != 2)
    {
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    //Parse arguments
    struct arguments args;
    if (parseURL(argv[1], &args) != 0)
    {
        return -1;
    }

    printInfo(&args);

    char *ip_address;
    ip_address = get_ip_address(args.host);

    printf("IP Address: %s\n", ip_address);

    //Create and open socket
    struct ftp ftp_connection;

    ftp_connection.sockfd = open_socket(21, ip_address);

    if (ftp_connection.sockfd < 0)
    {
        printf("Error opening socket!\n");
        return -1;
    }

    if (read_from_socket(ftp_connection.sockfd, buffer, sizeof(buffer)))
    {
        perror("ftp_read_from_socket()");
        return -1;
    }

    if (login(&ftp_connection, args.user, args.password))
    {
        perror("login error");
        return -1;
    }

    if (strlen(args.filePath) > 0)
    {
        if (change_directory(&ftp_connection, args.filePath))
        {
            perror("change directory");
            return -1;
        }
    }

    if (passive_mode(&ftp_connection))
    {
        perror("passive mode");
        return -1;
    }

    if (retrieve(&ftp_connection, args.fileName))
    {
        perror("retrieve");
        return -1;
    }

    if (download(&ftp_connection, args.fileName))
    {
        perror("download");
        return -1;
    }

    if (close_socket(&ftp_connection))
    {
        perror("disconnect");
        return -1;
    }

    return 0;
}
