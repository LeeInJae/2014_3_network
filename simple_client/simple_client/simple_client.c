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
		errorPrint("���Ӷ��̺귯�� �ʱ�ȭ ����");

	SOCKET clientSock;
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET)
		errorPrint("���� ���� ����");

	SOCKADDR_IN serverAddr;

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(IP);
	serverAddr.sin_port = htons(PORT);

	if (connect(clientSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		errorPrint("connect ����");

	char sendBuf[MAX_DATA];
	int sendSize;
	memset(sendBuf, 0, sizeof(sendBuf));
	strcpy_s(sendBuf, sizeof(sendString), sendString);

	sendSize = send(clientSock, sendBuf, sizeof(sendString), 0);
	if (sendSize == SOCKET_ERROR)
		errorPrint("send ����");

	printf("���� size = %d\n", sendSize);
	printf("���� data = %s\n", sendBuf);
	closesocket(clientSock);
	WSACleanup();
	getchar();
	return 0;
}