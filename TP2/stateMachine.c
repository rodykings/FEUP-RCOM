#include "stateMachine.h"

int stateMachine(int res, int fd, char buf[]){

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
    return state;
}
