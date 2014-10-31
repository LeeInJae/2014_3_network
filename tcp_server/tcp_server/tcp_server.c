#include <stdio.h>
#include <WinSock2.h>

#define PORT 3000
#define MAX_DATA 1000

int main(int argc, char * argv[]){
	WSADATA wsaData;
	SOCKET serverSocket, acceptSocket; 
	SOCKADDR_IN serverAddr, clientAddr;
	int checkErr, sizeClientAddr, recvCount = 0;
	char recvBuf[MAX_DATA], recvBufSize;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		perror("initiate winsock");
		return 0;
	}
	
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET){
		perror("socket");
		return 0;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORT);

	checkErr = bind(serverSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (checkErr == SOCKET_ERROR){
		perror("bind");
		return 0;
	}

	checkErr = listen(serverSocket, 1);
	if (checkErr == SOCKET_ERROR){
		perror("listen");
		return 0;
	}
	sizeClientAddr = sizeof(clientAddr);
	acceptSocket = accept(serverSocket, (SOCKADDR *)&clientAddr, &sizeClientAddr);
	if (acceptSocket == SOCKET_ERROR){
		perror("accept");
		return 0;
	}

	while (recvBufSize = recv(acceptSocket, &recvBuf[recvCount], 1, 0)){
		if (recvBufSize == SOCKET_ERROR){
			perror("recv");
			return 0;
		}
		++recvCount;
	}

	printf("%s\n", recvBuf);
	printf("recv count = %d\n", recvCount);
	closesocket(serverSocket);
	closesocket(acceptSocket);
	WSACleanup();
	getchar();
	return 0;
}