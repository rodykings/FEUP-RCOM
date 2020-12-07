#include "utils.h"
#include "clientTCP.h"

//Example: ./download ftp://ftp.up.pt/pub/file.md

int main(int argc, char *argv[])
{
    if (argc > 4 || argc < 2)
    {
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        return -1;
    }

    //Parse arguments
    struct arguments args;
    if (parseURL(argv[1], &args) != 0) {
        return -1;
    }

    printf("User: %s\n", args.user);
    printf("Password: %s\n", args.password);
    printf("Host: %s\n", args.host);
    printf("File Path: %s\n", args.filePath);
    printf("File Name: %s\n", args.fileName);

    char *ip_address;
    ip_address = get_ip_address(args.host);

    printf("IP Address: %s\n", ip_address);


    //Create and open socket
    if(open_socket(21, ip_address)<0){
        printf("Error opening socket!\n");
        return -1;
    }

    return 0;
}
