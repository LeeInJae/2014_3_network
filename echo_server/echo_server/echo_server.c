#include <stdio.h>
#include <WinSock2.h>

#define PORT 3000 //server가 사용할 임의의 포트 번호를 지정
#define MAX_DATA 1000 //주고받을 수 있는 데이터버퍼의 최대 용량을 임의로 지정

int main(int argc, char * argv[]){
	WSADATA wsaData;  //윈도우 소켓의 초기화를 위한 윈속데이터 구조체 변수선언
	SOCKET serverSocket, acceptSocket; //서버에서 필요한 소켓인 server 소켓과 accept소켓을 선언
	SOCKADDR_IN serverAddr, clientAddr; //서버의 주소와, 데이터를 주고받을 클라이언트의 주소정보를 담기위한 구조체 변수 각각 선언
	int checkErr, sizeClientAddr; //error 체크를 위한 변수와, client 주소의 크기를 저장할 변수를 선언
	char recvBuf[MAX_DATA], recvBufSize, sendBuf[MAX_DATA], sendBufSize; //데이터를 주고받을 때 사용할 recvBuf,size, sendBuf,size를 각각 선언.

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){  //윈도우 소켓의 버전을 확인하고, 초기화하는 작업.
		perror("initiate winsock");
		return 0;
	}

	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//서버소켓의 생성(IPv4체계, TCP소켓과, TCP프로토콜) 타입

	if (serverSocket == INVALID_SOCKET){ //소켓의 생성의 오류 확인
		perror("socket");
		return 0;
	}

	serverAddr.sin_family = AF_INET; //소켓에 주소체계의 저장(IPv4 인터넷 프로토콜 주소체계)
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP주소 초기화.
	serverAddr.sin_port = htons(PORT); //PORT 초기화

	checkErr = bind(serverSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)); //서버소켓에 속성에 해당하는 값들을 주소구조체를 이용하여, 할당해줌.

	if (checkErr == SOCKET_ERROR){ //bind의 에러여부 확인.
		perror("bind");
		return 0;
	}

	checkErr = listen(serverSocket, 1); //연결대기 큐의 크기가 1인 상태로, 서버소켓에서 연결요청을 기다림.
	if (checkErr == SOCKET_ERROR){ //listen의 에러여부 확인
		perror("listen");
		return 0;
	}

	sizeClientAddr = sizeof(clientAddr); //연결요청이 들어온 클라이언트의  주소정보 크기를 담을 변수
	acceptSocket = accept(serverSocket, (SOCKADDR *)&clientAddr, &sizeClientAddr); 
	//accept를 통하여 연결요청대기상태에 있는 클라이언트와 연결 후, accept소켓을 만듦

	if (acceptSocket == SOCKET_ERROR){//accept 소켓의 에러여부 확인
 		perror("accept");
		return 0;
	}

	memset(recvBuf, 0, MAX_DATA); //recv할 buffer 초기화
	memset(sendBuf, 0, MAX_DATA); //send할 buffer 초기화
	while (recvBufSize = recv(acceptSocket, &recvBuf, sizeof(recvBuf), 0)){ //클라이언트에서 보낸 데이터를 recv를 통해 recvBuf에 받음
 		if (recvBufSize == SOCKET_ERROR){ //recv를 제대로 했는지 에러여부 확인
			perror("recv");
			break;
		}

		strcpy_s(sendBuf,sizeof(recvBuf), recvBuf); //recvBuf에 받은 데이터를 그대로 send해야하므로, sendBuf에 복사
		checkErr = send(acceptSocket, sendBuf, strlen(sendBuf), 0);//연결된 클라이언트쪽으로 데이터 전송
		
		if (checkErr == -1){ //send를 제대로 했는지 에러여부 확인
			perror("send");
			break;
		}
	}

	closesocket(serverSocket); //서버소켓을 소멸시켜줌
	closesocket(acceptSocket); //accept 소켓을 소멸시켜줌
	WSACleanup();//윈도우 소켓관련 라이브러리를 운영체제에 반환
	getchar();
	return 0;
}