#include "receiver.h"

void setReceiver(int fd)
{
    int *size = malloc(sizeof(int));

    stateMachine(fd, A_TRM, C_SET, S, size);

    printf("\nTrama SET recebida\n");
    sendControlMsg(fd, A_TRM, C_UA);
    printf("\nTrama UA enviada\n");
}

fileInfo receiveControlPackage(int fd)
{

    int *sizeControlPackage = malloc(sizeof(int));
    unsigned char *controlPackage = stateMachine(fd, A_TRM, 0x00, I, sizeControlPackage);

    fileInfo fileinfo;
    int controlPackageStatus = checkControlPackage(controlPackage, sizeControlPackage, &fileinfo);

    printf("\nTrama I de controlo recebida - STATUS: %x\n", controlPackageStatus);

    return fileinfo;
};

int checkControlPackage(unsigned char *controlPackage, int *size, fileInfo *fileinfo)
{
    //Filesize field
    if (controlPackage[1] == 0)
    {
        int fileSize = 0;
        int shift = 24;
        for (int i = 3; i < controlPackage[2] + 3; i++)
        {
            fileSize |= (int)controlPackage[i] << shift;
            shift -= 8;
        }
        fileinfo->size = fileSize;
    }

    //Filename field
    if (controlPackage[7] == 1)
    {
        unsigned char *fileName = malloc(sizeof(unsigned char) * controlPackage[8]);
        int counter = 0;

        for (int i = 9; i < controlPackage[8] + 9; i++)
        {
            int nameSize = 0;
            fileName[counter++] = controlPackage[i];
        }

        fileinfo->filename = fileName;
    }

    return controlPackage[0];
}

void createFile(fileInfo info, unsigned char *fileData)
{
    char path[9 + strlen(info.filename)];
    strcpy(path, "received_");
    strcat(path, info.filename);
    printf("PATH: %s\n", path);
    FILE *fp = fopen(path, "wb+");

    fwrite((void *)fileData, 1, info.size, fp);
    fclose(fp);
}

void handleDisconnection(int fd)
{
    int *size = malloc(sizeof(int));

    stateMachine(fd, A_TRM, C_DISC, S, size);
    printf("Trama DISC recebida!\n");
    sendControlMsg(fd, A_REC, C_DISC);
    printf("Trama DISC enviada!\n");

    stateMachine(fd, A_REC, C_UA, S, size);
    printf("Trama UA recebida!\n");

    printf("Connection closed!\n");
}