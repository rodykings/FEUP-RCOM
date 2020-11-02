#include "receiver.h"

void setReceiver(int fd)
{
    int * size = malloc(sizeof(int));

    stateMachine(fd, C_SET, S, size);
    
    printf("\nTrama SET recebida\n");
    sendControlMsg(fd, C_UA);
    printf("\nTrama UA enviada\n");
} 

fileInfo receiveControlPackage(int fd){

    int* sizeControlPackage = malloc(sizeof(int));
    unsigned char* controlPackage = stateMachine(fd, 0x00, I, sizeControlPackage);

    fileInfo fileinfo;
    int controlPackageStatus = checkControlPackage(controlPackage, sizeControlPackage, &fileinfo);

    printf("Nome ficheiro: %s\n", fileinfo.filename);
    printf("Tamanho ficheiro: %d\n", fileinfo.size);
    
    printf("\nTrama I de controlo recebida - STATUS: %x\n", controlPackageStatus);

   //createFile(fileinfo);

    return fileinfo;
};

int checkControlPackage(unsigned char*controlPackage, int*size, fileInfo* fileinfo){
    //Filesize field
    if(controlPackage[1] == 0){
        int fileSize = 0;
        int shift = 24;
        for(int i=3; i<controlPackage[2]+3;i++){
            fileSize |= (int) controlPackage[i] << shift;
            shift-=8;
        }
        fileinfo->size = fileSize;
    }

    //Filename field
    if(controlPackage[7] == 1){
        unsigned char * fileName = malloc(sizeof(char) * controlPackage[8]);
        int counter = 0;
        
        for(int i=9; i<controlPackage[8]+9;i++){
            int nameSize = 0;
            fileName[counter++] = controlPackage[i];
        }

        fileinfo->filename = fileName;
    }    

    return controlPackage[0];
}

void createFile(fileInfo info, unsigned char* fileData){
    FILE *fp = fopen("file.gif", "wb");

    
    //fseek(fp, info.size , SEEK_SET);
   // printf("CREI UM FICHEIRO COM %d bytes" , info.size);
    fwrite(fileData, sizeof(unsigned char), info.size, fp);
    //fputc('\0', fp);
    fclose(fp);
}