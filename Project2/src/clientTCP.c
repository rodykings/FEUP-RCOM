#include "clientTCP.h"

int open_socket(int port, char *address)
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

	printf("Socket successfully opened!\n\n");

	return sockfd;
}

int write_to_socket(int fd, char *buf, size_t size)
{
	int bytes;

	if ((bytes = write(fd, buf, size)) <= 0)
	{
		printf("WARNING: Error sending to server.\n");
		return 1;
	}

	printf("Bytes send: %d\nInfo: %s\n", bytes, buf);

	return 0;
}

int read_from_socket(int fd, char *buf, size_t buf_size)
{
	FILE *fp = fdopen(fd, "r");

	do
	{
		memset(buf, 0, buf_size);
		buf = fgets(buf, buf_size, fp);
		printf("%s", buf);

	} while (!('1' <= buf[0] && buf[0] <= '5') || buf[3] != ' ');

	return 0;
}

int login(struct ftp *ftp_connection, char *user, char *password)
{

	char buffer[MAX_SIZE];

	sprintf(buffer, "USER %s\r\n", user);

	if (write_to_socket(ftp_connection->sockfd, buffer, strlen(buffer)))
	{
		printf("ERROR: TCP send fail\n");
		return -1;
	}

	if (read_from_socket(ftp_connection->sockfd, buffer, sizeof(buffer)))
	{
		printf("Access denied: failed reading from server!\n");
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "PASS %s\r\n", password);

	if (write_to_socket(ftp_connection->sockfd, buffer, strlen(buffer)))
	{
		printf("ERROR: TCP send fail\n");
		return -1;
	}

	if (read_from_socket(ftp_connection->sockfd, buffer, sizeof(buffer)))
	{
		printf("Access denied: failed reading from server!\n");
		return -1;
	}

	return 0;
}

int passive_mode(struct ftp *ftp_connection)
{
	char passive[MAX_SIZE] = "PASV\r\n";
	char ip_address[MAX_SIZE];
	int port;

	if (write_to_socket(ftp_connection->sockfd, passive, strlen(passive)))
	{
		printf("Failed entering passive mode!\n");
		return -1;
	}

	if (read_from_socket(ftp_connection->sockfd, passive, sizeof(passive)))
	{
		printf("Not getting response entering in passive mode!\n");
		return -1;
	}

	if (parse_passive_response(passive, ip_address, &port))
	{
		perror("parse_passive_response failed!\n");
		return -1;
	}

	printf("IP Address: %s\n", ip_address);
	printf("Port: %d\n", port);

	ftp_connection->datafd = open_socket(port, ip_address);
	if (ftp_connection->datafd < 0)
	{
		printf("Error opening socket!\n");
		return -1;
	}

	printf("Entered passive mode successfully.\n");
	return 0;
}

int change_directory(struct ftp *ftp_connection, char *path)
{
	char change_directory[MAX_SIZE];

	sprintf(change_directory, "CWD %s\r\n", path);
	if (write_to_socket(ftp_connection->sockfd, change_directory, strlen(change_directory)))
	{
		printf("Failed sending path to CWD.\n");
		return -1;
	}

	if (read_from_socket(ftp_connection->sockfd, change_directory, sizeof(change_directory)))
	{
		printf("Failed changing directory.\n");
		return -1;
	}

	return 0;
}

int retrieve(struct ftp *ftp_connection, char *file)
{
	char retr[MAX_SIZE];

	sprintf(retr, "RETR %s\r\n", file);
	if (write_to_socket(ftp_connection->sockfd, retr, strlen(retr)))
	{
		printf("Error sending filename\n");
		return -1;
	}

	if (read_from_socket(ftp_connection->sockfd, retr, sizeof(retr)))
	{
		printf("Filename not received!\n");
		return -1;
	}

	return 0;
}

int download(struct ftp *ftp_connection, char *filename)
{
	FILE *file;
	int bytes;

	if (!(file = fopen(filename, "w")))
	{
		printf("ERROR: Cannot open file.\n");
		return 1;
	}

	char buf[MAX_SIZE];
	printf("Starting to download file with name %s\n", filename);
	while ((bytes = read(ftp_connection->datafd, buf, sizeof(buf))))
	{
		if (bytes < 0)
		{
			printf("ERROR: Nothing was received from data socket fd.\n");
			return -1;
		}

		if ((bytes = fwrite(buf, bytes, 1, file)) < 0)
		{
			printf("ERROR: Cannot write data in file.\n");
			return -1;
		}
	}
	if (fclose(file) < 0)
	{
		printf("Error closing file\n");
		return -1;
	}

	close(ftp_connection->datafd);

	return 0;
}

int close_socket(struct ftp *ftp_connection)
{
	char buffer[MAX_SIZE];

	if (read_from_socket(ftp_connection->sockfd, buffer, sizeof(buffer)))
	{
		printf("Can't disconnect.\n");
		return 1;
	}

	sprintf(buffer, "QUIT\r\n");

	if (write_to_socket(ftp_connection->sockfd, buffer, strlen(buffer)))
	{
		printf("Failed disconnection!\n");
		return -1;
	}

	if (ftp_connection->sockfd)
		close(ftp_connection->sockfd);

	return 0;
}
