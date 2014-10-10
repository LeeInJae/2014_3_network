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

#define ERROR_NUMBER 1
#define IP "127.0.0.1"
#define PORT 3000
#define MAX_DATA 100
#define WRITE_DATA "LeeInJae HIHI"

void errorPrint(char * message){
    printf("%s\n",message);
    exit(ERROR_NUMBER);
}

int main(int argc, const char * argv[])
{
    int clientSock;
    
    clientSock = socket(PF_INET, SOCK_STREAM, 0);
    if(clientSock == - 1)
        errorPrint("소켓 생성 실패");
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IP);
    serverAddr.sin_port = htons(PORT);
    
    if(connect(clientSock,(struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        errorPrint("connect 실패");
    
    ssize_t writeSize;
    char writeBuf[MAX_DATA];
    memset(writeBuf,0,sizeof(writeBuf));
    strcpy(writeBuf, WRITE_DATA);
    writeSize = write(clientSock, writeBuf, sizeof(WRITE_DATA));
    if(writeSize == -1)
        errorPrint("write 실패");
    
    printf("보낸 바이트 수 %ld\n", writeSize);
    printf("%s\n", writeBuf);
    
    close(clientSock);
    
    getchar();
    return 0;
}


