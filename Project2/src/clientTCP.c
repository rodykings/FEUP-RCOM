#include "clientTCP.h"

int open_socket(int port, char* address)
{
	int sockfd;
	struct sockaddr_in server_addr;

	/*server address handling*/
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(address); /*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);				  /*server TCP port must be network byte ordered */

	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket()");
		return -1;
	}
	/*connect to the server*/
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect()");
		return -1;
	}

	printf("Socket successfully opened!\n");

	return sockfd;
}

int write_to_socket(int sockfd){

	return 0;
}

int read_from_socket(int sockfd){

	return 0;
}


int close_socket(int sockfd){
	close(sockfd);
	return 0;
}

/*----------------------GIVEN CODE------------------*/

// int main(int argc, char** argv){

// 	int	sockfd;
// 	struct	sockaddr_in server_addr;
// 	char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
// 	int	bytes;
	
// 	/*server address handling*/
// 	bzero((char*)&server_addr,sizeof(server_addr));
// 	server_addr.sin_family = AF_INET;
// 	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
// 	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
    
// 	/*open an TCP socket*/
// 	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
//     		perror("socket()");
//         	exit(0);
//     	}
// 	/*connect to the server*/
//     	if(connect(sockfd, 
// 	           (struct sockaddr *)&server_addr, 
// 		   sizeof(server_addr)) < 0){
//         	perror("connect()");
// 		exit(0);
// 	}
//     	/*send a string to the server*/
// 	bytes = write(sockfd, buf, strlen(buf));
// 	printf("Bytes escritos %d\n", bytes);

// 	close(sockfd);
// 	exit(0);
// }

