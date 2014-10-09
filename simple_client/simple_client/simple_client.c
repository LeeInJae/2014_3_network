#include<stdio.h>
#include <winsock2.h>

#define MAX_DATA 100
#define ERROR_NUMBER 1
#define IP "127.0.0.1"
#define PORT 3000
#define sendString "LeeInJae"

void errorPrint(char * message){
	printf("%s\n", message);
	getchar();
	exit(ERROR_NUMBER);
}

int main(){
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		errorPrint("윈속라이브러리 초기화 실패");

	SOCKET clientSock;
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET)
		errorPrint("소켓 생성 실패");

	SOCKADDR_IN serverAddr;

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(IP);
	serverAddr.sin_port = htons(PORT);

	if (connect(clientSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		errorPrint("connect 실패");

	char sendBuf[MAX_DATA];
	int sendSize;
	memset(sendBuf, 0, sizeof(sendBuf));
	strcpy_s(sendBuf, sizeof(sendString), sendString);

	sendSize = send(clientSock, sendBuf, sizeof(sendString), 0);
	if (sendSize == SOCKET_ERROR)
		errorPrint("send 실패");

	printf("보낸 size = %d\n", sendSize);
	printf("보낸 data = %s\n", sendBuf);
	closesocket(clientSock);
	WSACleanup();
	getchar();
	return 0;
}