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

#define ERROR_NUMBER 1
#define PORT 3000
#define MAX_DATA 100

void errorPrint(char * message){
    printf("%s\n",message);
    exit(ERROR_NUMBER);
}

int main(int argc, const char * argv[])
{
    int serverSock = socket(PF_INET, SOCK_STREAM, 0);
    if(serverSock == -1)
        errorPrint("소켓 생성 실패");
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = (INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    
    if(bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        errorPrint("바인드 실패");
    
    if(listen(serverSock, 10) == -1)
        errorPrint("listen 실패");
    
    int clientSock;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if(clientSock == -1)
        errorPrint("accept 실패");
    
    char readBuf[MAX_DATA];
    memset(readBuf,0,sizeof(readBuf));
    ssize_t readSize;
    readSize = read(clientSock, readBuf, sizeof(readBuf));
    if(readSize == -1)
        errorPrint("read실패");
    
    printf("읽은 바이트 수 %ld\n", readSize);
    printf("%s\n", readBuf);
    
    close(clientSock);
    close(serverSock);
    
    getchar();
    return 0;
}
