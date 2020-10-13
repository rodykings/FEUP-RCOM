/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


#define FLAG 0x01111110;
#define A_EM_REC_CMD 0x03;
#define A_REC_EM_CMD 0x01;

volatile int STOP=FALSE;

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP_UA} UA_State_Machine;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    
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

    newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN] = 1;   /* blocking read until 1 chars received */

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

    printf("Write whatever you want: ");
    fgets(buf, 255, stdin);

    int nchar = 0;
    for(int i=0; i < 256; i++){
      if(buf[i]=='\0'){
        nchar = i;
        break;
      }
        
    }
  
    /*Aqui temos que enviar a mensagem no formato FLAG A C BCC FLAG*/
    /*nao estou a ver como se envia XD*/

    unsigned char set[5];
    set[0] = FLAG;
    set[1] = A_EM_REC_CMD;
    set[2] = C;
    set[3] = A_EM_REC_CMD ^ C;
    set[4] = FLAG;
    
    //O BCC É O OU LÓGICO ENTRE O A E O C 
    //NAO ESTOU A ENCONTRAR O VALOR DE C :(
    //tmbém não, vou ver se alguém sabe e me consegue explicar!!

    res = write(fd,set,8);
    
    //res = write(fd,buf,nchar);   
    printf("%d characters written\n", nchar);
 

  /* 
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar 
    o indicado no gui�o 
  */

   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}
