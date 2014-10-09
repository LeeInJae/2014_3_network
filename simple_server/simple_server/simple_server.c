#include <stdio.h>
#include <winsock2.h>

#define MAX_DATA 100
#define ERROR 1
#define PORT 3000

//socket 만들고
//bind 해서 할당해주고
//listen으로 듣고
//accept해서 받고

void errorPrint(char * message){
	printf("%s\n", message);
	getchar();
	exit(ERROR);
}

int main(){
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		errorPrint("윈속라이브러리 초기화 실패");

	SOCKET serverSock;
	serverSock = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET)
		errorPrint("소켓 생성 실패");

	SOCKADDR_IN servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);

	if (bind(serverSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		errorPrint("소켓에 바인드를 하지 못함");

	if (listen(serverSock, 1) == SOCKET_ERROR)
		errorPrint("소켓 listen 함수 수행 실패");

	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	int sizeClientAddr = sizeof(clientAddr);

	clientSock = accept(serverSock, (SOCKADDR *)&clientAddr, &sizeClientAddr);
	if (clientSock == INVALID_SOCKET)
		errorPrint("accpet실패!");

	char buf[MAX_DATA];
	int recvSize;
	recvSize = recv(serverSock, buf, sizeof(buf), 0);
	
	if (recvSize == SOCKET_ERROR)
		errorPrint("recv 실패");
	
	printf("받은 size = %d\n", recvSize);
	printf("%s\n", buf);

	closesocket(clientSock);
	closesocket(serverSock);
	WSACleanup();
	getchar();
	return 0;
}