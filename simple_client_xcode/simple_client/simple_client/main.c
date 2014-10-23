//
//  main.c
//  simple_client
//
//  Created by 이인재 on 2014. 10. 10..
//  Copyright (c) 2014년 이인재. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 3000
#define MAX_DATA 100
#define WRITE_DATA "Welcome ~!"


int main(int argc, const char * argv[])
{
    int ret = -1;
    int clientSock;
    ssize_t writeSize;
    char writeBuf[MAX_DATA];
    struct sockaddr_in serverAddr;
   
    clientSock = socket(PF_INET, SOCK_STREAM, 0);
    if(clientSock == - 1){
        perror("socket");
        goto leave;
    }
   
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IP);
    serverAddr.sin_port = htons(PORT);
    
    if((ret = connect(clientSock,(struct sockaddr*)&serverAddr, sizeof(serverAddr)))){
        perror("socket");
        goto leave;
    }
    
    strcpy(writeBuf, WRITE_DATA);
    writeSize = write(clientSock, writeBuf, sizeof(WRITE_DATA));
    if(writeSize == -1){
        perror("write");
        goto leave;
    }
    
    printf("보낸 바이트 수 %ld\n", writeSize);
    printf("%s\n", writeBuf);
    
    close(clientSock);
    
    getchar();
leave:
    return ret;
}


