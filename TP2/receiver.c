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
#define C 0x03

volatile int STOP=FALSE;

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP_UA} UA_State_Machine;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");


    UA_State_Machine state = START;

    res = read(fd,buf,1);   /* returns after 1 chars have been input */
    while (STOP==FALSE) {       /* loop for input */
      printf("STATE: %d\n", state);
      switch(state){
        case START:
          if(buf[0] == FLAG){
            state = FLAG_RCV;
          }
          res = read(fd,buf,1); //reads next char  
          break;
        case FLAG_RCV:
          if(buf[0] == A)
          {
            state = A_RCV;
          }
          else if(buf[0] == FLAG){
            state = FLAG_RCV;
          }
          else state = START;
          res = read(fd,buf,1);   
          break;
        case A_RCV:
          if(buf[0] == C)
          {
            state = C_RCV;
          }
          else if(buf[0] == FLAG){
            state = FLAG_RCV;
          }
          else state = START;
          res = read(fd,buf,1);   
          break;
        case C_RCV:
          if(buf[0] == (A || C))
          {
            state = BCC_OK;
          }
          else if(buf[0] == FLAG){
            state = FLAG_RCV;
          }
          else state = START;
          res = read(fd,buf,1);   
          break;
        case BCC_OK:
          if(buf[0] == FLAG){
            state = STOP_UA;
          }
          else state = START;
          break;
        case STOP_UA:
          printf("TRAMA CORRETA\n");
          STOP = TRUE;
      }
    }

    



  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
