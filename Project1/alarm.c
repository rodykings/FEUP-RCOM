#include "alarm.h"

// int flag=1, conta=1;

// void atende()                   // atende alarme
// {
// 	printf("alarme # %d\n", conta);
// 	flag=1;
// 	conta++;
// }


// main()
// {

// (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

// while(conta < 4){
//    if(flag){
//       alarm(3);                 // activa alarme de 3s
//       flag=0;
//    }
// }
// printf("Vou terminar.\n");

// }

int finish, num_retry = 0;

void alarmHandler(int signal){
    if(num_retry >= MAX_RETRY){
        finish = 1;
        printf("End of connection!");
        return;
    }
    else{
        num_retry++;
    }
}


