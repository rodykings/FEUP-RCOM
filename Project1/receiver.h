/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

struct termios oldtio,newtio;
typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP} Set_State_Machine;

int receiveControlMsg(int fd, unsigned char c);
void sendControlMsg(int fd, unsigned char C);
void LLOPEN(int fd);
int main(int argc, char **argv);