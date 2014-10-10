//
//  main.c
//  simle_server
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

#define PORT 3000
#define MAX_DATA 100

int main(int argc, const char * argv[])
{
    int ret = -1;
    struct sockaddr_in serverAddr;
    int serverSock;
    int acceptedSock;
    struct sockaddr_in clientAddr;
    char readBuf[MAX_DATA];
    ssize_t readSize;
    socklen_t clientAddrSize = sizeof(clientAddr);
    
    serverSock = socket(PF_INET, SOCK_STREAM, 0);
    if(serverSock == -1) {
        perror("socket");
        goto leave;
    }
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = (INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    
    if((ret = bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))){
        perror("bind");
        goto leave;
    }
    
    if(listen(serverSock, 1) == -1){
        perror("listen");
        goto leave;
    }
    
    acceptedSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if(acceptedSock == -1){
        perror("accept");
        goto leave;
    }
    
    readSize = read(acceptedSock, readBuf, MAX_DATA);
    if(readSize == -1){
        perror("read");
        goto leave;
    }
    
    printf("읽은 바이트 수 %ld\n", readSize);
    printf("%s\n", readBuf);
    
    close(acceptedSock);
    close(serverSock);

    getchar();
leave:
    return ret;
}
