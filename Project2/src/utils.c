#include "utils.h"

int verify_ftp(const char *str)
{

    char *ftp_str = "ftp://";
    char aux[7];

    memcpy(aux, str, 6);

    return !strcmp(ftp_str, aux);
}

int parseURL(char *complete_url, struct arguments *args)
{
    // verify ftp
    char start[6];
    strncpy(start, complete_url, 6);
    complete_url += 6;
    if (!verify_ftp(start))
    {
        printf("URL must start with ftp://\n");
        return -1;
    }

    char *token = strtok(complete_url, "\0");
    char url[MAX_SIZE];
    strcpy(url, token);

    //reads user
    char aux[MAX_SIZE];
    strcpy(aux, url);

    token = strtok(aux, ":");

    if (token == NULL)
    {
        printf("Failed parsing user!\n");
        return -1;
    }
    else if (strcmp(token, url) == 0) //User is not indicated
    {
        memset(args->user, 0, sizeof(args->user));
        strcpy(args->user, "anonymous");
        memset(args->password, 0, sizeof(args->password));
        strcpy(args->password, "");

        char aux2[MAX_SIZE];
        strcpy(aux2, &url[0]);
        strcpy(url, aux2);
    }
    else
    {
        memset(args->user, 0, sizeof(args->user));
        strcpy(args->user, &token[2]);

        //read passowrd
        token = strtok(NULL, "@");
        if (token == NULL || (strlen(token) == 0))
        {
            printf("Failed parsing password\n");
            return -1;
        }
        memset(args->password, 0, sizeof(args->password));
        strcpy(args->password, token);

        token = strtok(NULL, "\0");
        strcpy(url, token);
    }

    //read host
    token = strtok(url, "/");
    if (token == NULL)
    {
        printf("Failed parsing host\n");
        return -1;
    }
    memset(args->host, 0, sizeof(args->host));
    strcpy(args->host, token);

    //get path and name
    token = strtok(NULL, "\0");
    if (token == NULL)
    {
        printf("Failed parsing file path\n");
        return -1;
    }
    char *name = strrchr(token, '/');
    if (name != NULL)
    {
        //get path
        memset(args->filePath, 0, sizeof(args->filePath));
        strncpy(args->filePath, token, name - token);

        //get name
        memset(args->fileName, 0, sizeof(args->fileName));
        strcpy(args->fileName, name + 1);
    }
    else //only filename is indicated
    {
        memset(args->filePath, 0, sizeof(args->filePath));
        strcpy(args->filePath, "");
        memset(args->fileName, 0, sizeof(args->fileName));
        strcpy(args->fileName, token);
    }

    return 0;
}

char *get_ip_address(char *host_name)
{
    struct hostent *h;
    /*
    struct hostent {
	char    *h_name;	Official name of the host. 
    char    **h_aliases;	A NULL-terminated array of alternate names for the host. 
	int     h_addrtype;	The type of address being returned; usually AF_INET.
    int     h_length;	The length of the address in bytes.
	char    **h_addr_list;	A zero-terminated array of network addresses for the host. 
				Host addresses are in Network Byte Order. 
};
#define h_addr h_addr_list[0]	The first address in h_addr_list. 
*/
    if ((h = gethostbyname(host_name)) == NULL)
    {
        herror("gethostbyname");
        exit(1);
    }

    return inet_ntoa(*((struct in_addr *)h->h_addr_list[0]));
}
