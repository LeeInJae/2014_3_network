#include <stdio.h>
#include <winsock2.h>

#define MAX_DATA 100
#define ERROR_NUMBER 1
#define PORT 3000

//socket �����
//bind �ؼ� �Ҵ����ְ�
//listen���� ���
//accept�ؼ� �ް�

//������Ʈ �Ӽ� -> ��Ŀ -> �Է� -> �߰����Ӽ� -> edit -> ws2_32.lib
void errorPrint(char * message){
	printf("%s\n", message);
	getchar();
	exit(ERROR_NUMBER);
}

int main(){
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		errorPrint("���Ӷ��̺귯�� �ʱ�ȭ ����");

	SOCKET serverSock;
	serverSock = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET)
		errorPrint("���� ���� ����");

	SOCKADDR_IN servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);

	if (bind(serverSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		errorPrint("���Ͽ� ���ε带 ���� ����");

	if (listen(serverSock, 1) == SOCKET_ERROR)
		errorPrint("���� listen �Լ� ���� ����");

	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	int sizeClientAddr = sizeof(clientAddr);

	clientSock = accept(serverSock, (SOCKADDR *)&clientAddr, &sizeClientAddr);
	if (clientSock == INVALID_SOCKET)
		errorPrint("accpet����!");

	char buf[MAX_DATA];
	int recvSize;
	recvSize = recv(clientSock, buf, sizeof(buf), 0);
	
	if (recvSize == SOCKET_ERROR)
		errorPrint("recv ����");
	
	printf("���� size = %d\n", recvSize);
	printf("%s\n", buf);

	closesocket(clientSock);
	closesocket(serverSock);
	WSACleanup();
	getchar();
	return 0;
}