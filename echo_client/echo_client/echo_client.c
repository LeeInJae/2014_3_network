#include <stdio.h>
#include <WinSock2.h>

#define IP "127.0.0.1"
#define PORT 3000
#define MAX_DATA 1000

int main(int argc, char * argv[]){
	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	int checkErr, recvCount;
	char sendBuf[MAX_DATA], sendBufSize = 0, recvBuf[MAX_DATA], recvBufSize = 0, sendCount = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		perror("initiate winsock");
		return 0;
	}

	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == SOCKET_ERROR){
		perror("initiate winsock");
		return 0;
	}

	serverAddr.sin_family = AF_INET; //연결 요청을 할 서버의 주소ㅗ기화체계 초기화
	serverAddr.sin_addr.s_addr = inet_addr(IP); //연결 요청을 할 서버의 IP주소 초기화
	serverAddr.sin_port = htons(PORT); //연결 요청을 할 서버의 포트번호 초기화

	checkErr = connect(clientSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)); //연결요청할 서버의 주소정보가 담긴 구조체를 이용하여 연결요청
	if (checkErr == SOCKET_ERROR){ //연결요청 시도의 에러여부 확인
		perror("socket");
		return 0;
	}

	while (1){
		fgets(sendBuf, MAX_DATA, stdin); //fgets를 통해, 서버로 보낼 문자열 입력
		sendBuf[strlen(sendBuf) - 1] = '\0'; //fgets는 엔터키 '\n' 개행도 포함하므로 임의로 '\0'로 채워줌
		
		if (strcmp(sendBuf, "Q") == 0) //Q가 입력으로 들어오면 통신 종료.
			break;

 		checkErr = send(clientSocket, sendBuf, strlen(sendBuf), 0); //client 에서 생성한 소켓을 이용하여 서버로 문자열을 보냄
		
		if (checkErr == -1){ //send의 에러 여부 확인
			perror("send");
			break;
		}
		
		memset(recvBuf,0,MAX_DATA); //서보로부터 받을 recvBuffer를 초기화
		recvCount = 0;

		while (recvCount < strlen(sendBuf)){ //클라이언트에서 보냈던 만큼의 데이터를 받기위한 조건문
			recvBufSize = recv(clientSocket, &recvBuf[recvCount], 1, 0); //recv를 이용하여 서버로부터 데이터를 1바이트씩 받음,그리고 순차적으로 recvBuf에 저장
			if (recvBufSize == SOCKET_ERROR){ //recv의 에러여부 확인
				perror("recv");
				break;
			}
			++recvCount; //이변수를 이용하여 몇바이트를 받았는지 확인.
		}
		printf("(client) 서버에서 받은 문자열 %s \n", recvBuf); //서버로부터 받은 데이터 확인
	}

	closesocket(clientSocket);
	WSACleanup();
	getchar();
	return 0;
}