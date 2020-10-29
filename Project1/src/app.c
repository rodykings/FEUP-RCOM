#include "protocol.h"

int main(int argc, char **argv)
{
    int status;
    if (argc == 3)
    {
        status = RECEIVER;
    }
    else if (argc == 4)
    {
        status = TRANSMITTER;
    }
    else
    {
        printf("Usage:\n");
        printf("\tTRANSMITTER: ./app /dev/sttyS0 send <filename>\n");
        printf("\tRECEIVER: ./app /dev/sstyS1 receive\n");
        return -1;
    }
    // ./app /dev/sttyS0 send "path"
    // ./app /dev/sstyS1 receive

    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */
    int fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(argv[1]);
        exit(-1);
    }

    llopen(fd, status);
    
    //llwrite(fd, argv[3]);
    //

    return 0;
}