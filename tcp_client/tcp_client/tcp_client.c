#include <stdio.h>
#include <WinSock2.h>

#define IP "127.0.0.1"
#define PORT 3000
#define MAX_DATA 1000
#define MESSAGE "hihi"

int main(int argc, char * argv[]){
	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	int checkErr;
	char sendBuf[MAX_DATA], sendBufSize = 0, sendCount = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 ){
		perror("initiate winsock");
		return 0;
	}

	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == SOCKET_ERROR){
		perror("initiate winsock");
		return 0;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(IP);
	serverAddr.sin_port = htons(PORT);

	checkErr = connect(clientSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (checkErr == SOCKET_ERROR){
		perror("socket");
		return 0;
	} 
	strcpy_s(sendBuf, sizeof(MESSAGE), MESSAGE);

	/*
	while (sendBufSize = send(clientSocket, sendBuf, sizeof(MESSAGE), 0)){
		if (sendBufSize == SOCKET_ERROR){
			perror("send");
			return 0;
		}
		++sendCount;
		if (sendCount == 100)
			break;
	}
	printf("%s\n", sendBuf);
	printf("send count = %d\n", sendCount);
	*/
	
	checkErr = send(clientSocket, sendBuf, sizeof(MESSAGE), 0);
	if (checkErr == -1){
		perror("send");
		return 0;
	}
	
	closesocket(clientSocket);
	WSACleanup();
	getchar();
	return 0;
}