#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

//멀티 스레드 환경으로 동시접속 서버 구현을 위한 헤더 파일 선언
#include <pthread.h>

/*
I/O reference counting을 위해서 BLOCK의 수와 BLOCK크기를 설정 
한개의 블락당 100Mb로 설정
블락의 최대 개수는 200개로 설정(최대 크기는 100 * 200 Mb크기의 파일)
*/
#define BLOCK_SIZE 1024*1024*100
#define BLOCK_MAX_COUNT 200

//에러 메시지 출력을 위한 함수
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}

//스레드가 실행하는 메인함수를 설정하고 이를 함수 포인터로 쓰기 위해 함수포인터를 이용한 메소드 선언
void * handle_clnt(void * arg);

int clnt_cnt = 0; //해당 프로세서(메인 프로세스) 에서 관리할 클라이언트 소켓의 총 수를 담기 위한 변수
int clnt_socks[256]; //해당 프로세서(메인 프로세스) 에서 관리하는 클라이언트 소켓의 정보들을 저장하는 배열

pthread_mutex_t mutx; /*멀티스레드 환경에서 실행되는 프로세스 이므로, 임계영역에 대한 락이 필요한 경우, mutex로 락을 걸어주기 위해
							글로벌 뮤텍스 락을 잡아줌 */

/* reference counting을 위해 사용하는 블록만 메모리에 올리도록 하려면 해당 정보를 담을 자료구조가 필요.
이 자료구조를 구조체로 만들어, 실제 블락에 대한 payload를 담을 buf, 현재 참조 카운팅인 ref변수, 해당 블락의 길이를 저장하는 buf_length변수를 
갖는 BLOCK구조체 선언
*/
struct BLOCK
{
	char* buf; //레퍼런스 카운팅을 통한, 동적할당 및 메모리 해제를 위하여 char* 로 buf 선언
	int ref; //레퍼런스 카운팅을 위한 변수
	int buf_length; //해당 블록의 길이를 저장할 변수
};

/*
서버에서 동적으로 파일에 대한 블락 설정을 위해, 파일 크기에 딱 맞는 수만큼 블락 크기를 구해야 한다.
따라서 파일의 블락 크기를 저장하는 end_block_count변수 선언
*/

unsigned int end_block_count;

//reference 카운팅을 위한 block에 대한 정보를 저장할 BLOCK 구조체의 배열을 선언

struct BLOCK block[BLOCK_MAX_COUNT];

//reference 카운팅을 위한 글로벌 파일포인터를 선언(스레드에서 직접 공유하며 사용하기위해)


int main(int argc, char *argv[]){
	FILE * fp;
	fp = fopen("abc.avi","rb"); //보내려고 하는 파일인 "abc.avi" 동영상 파일을 (약 1.29GB)를 일기형태로 열어 파일포인터를 얻음
	
	fseek(fp,0,SEEK_END); //해당 파일의 총 크기를 알기 위해 fseek함수를 이용하여 파일 포인터를 파일의 끝으로 이동
	int number1 = ftell(fp); //파일포인터의 현재위치를 반환하는 ftell함수를 통해서 보내려고 하는 파일의 총 크기를 알 수 있음
	int number2 = (int)BLOCK_SIZE; //블락 단위로 나눠 주기 위한 BLOCK 사이즈를 받아옴
	
	end_block_count = (int)number1 / (int)number2; //현재 파일이 총 몇개의 블락으로 나뉘는지 계산
	if( number1 % number2 != 0 ) // 만약 끝에 블락사이즈보다 작은 사이즈의 나머지 데이터가 남아 있다면 그 정보를 담을 블락 카운트 + 1( 쪼가리 데이터 처리)
		++end_block_count;
	
	printf( "endblock = %d\n", end_block_count); //블락사이즈 체크를 위한 출력 메시지
	fseek(fp,0,SEEK_SET); //파일 포인터를 처음 위치로 다시 원상 복귀 시킴(스레드에서 읽어가는 데이터는 처음부터 순차적으로 읽어가야 하므로)
	fclose(fp);
	for( int i=0; i<BLOCK_MAX_COUNT; ++i) //block배열을 초기화
	{
		block[i].buf = NULL; //buf 포인터를 NULL로 초기화
		block[i].ref = 0; //레퍼런스 카운트 0 으로 초기화
		block[i].buf_length = 0; //해당 블락의 길이를 0 으로 초기화
	}
	
	int serv_sd, clnt_sd; //tcp연결을 위한 소켓 end point생성
	
	struct sockaddr_in serv_adr; //주소 정보를 담을  변수 초기화
	struct sockaddr_in clnt_adr;
	
	socklen_t clnt_adr_sz; //클라이언트의 크기를 담을 변수
	
	pthread_t t_id;  //스레드의 생성과, 종료를 thread id로 관리할 변수 선언
	
	
	if(argc!=2){ //몇번 포트로 열것인지, 잘열렸는지 확인
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutx,NULL); //락을 걸기 위한 mutex락 초기화
	
	serv_sd = socket( PF_INET, SOCK_STREAM, 0 ); //서버사이드의 소켓 생성, IPv4 프로토콜, tcp 스트림 인터페이스를 제공하는 소켓
	
	if(serv_sd==-1) //소켓 생성에 실패하는지의 여부 체크
		error_handling("socket() error");

	memset(&serv_adr,0,sizeof(serv_adr)); //주소 정보를 담을 구조체 변수 초기화
	
	serv_adr.sin_family=AF_INET; //주소 정보를 IPv4체계로 저장
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); //소켓을 사용하는 컴퓨터의 ip주소를 자동 할당해주는 INADDR_ANY 사용
	serv_adr.sin_port=htons(atoi(argv[1]));//지정 포트를 설정하고, 네트워크의 바이트오더링으로 맞춤

	if(bind(serv_sd,(struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1) //bind로 소켓과 포트 정보를 연결
		error_handling("bind() error");//bind 함수의 성공여부 체크
	if(listen(serv_sd,10)==-1) //listen함수를 통하여 요청이 있는지 체크, 크기를 10개로 두어 정보 관리
		error_handling("listen() error");//listen함수의 성공여부 체크
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr); //clint정보를 표현하는 사이즈
		clnt_sd=accept(serv_sd,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);/* accept를 통해 connect를 요청한 client의 정보를 받고, 스트림형식의 데이터 교환												                              을 위한 파일 디스크립터를 받아옴 */
		
		
		if(clnt_sd==-1) //해당 하는 파일디스크립터(소켓)이 생성되지 않았는지 확인
			error_handling("accept() error");
		else
			printf("Connected client %d \n",clnt_cnt+1);
				
		pthread_mutex_lock(&mutx); //현재 프로세서에서 클라이언트들을 관리할 곳에 대해 락을 설정( 동시에 스레드가 쓸 수 없도록)
		clnt_socks[clnt_cnt++] = clnt_sd; //소켓들을 배열에 담아 관리
		pthread_mutex_unlock(&mutx);
		
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sd);//accept가 왔다면 새로운 스레드를 생성하고, handle_clnt의 함수로 소켓정보를 넘겨줌
		pthread_detach(t_id);//해당 스레드가 끝이 났다면 스레드를 소멸.
	}
	close(serv_sd);//연결이 끝났다면 서버소켓을 소멸 시킴
	//fclose(fp);//연결이 끝났다면 파일 포인터를 닫아줌
	
	return 0;
}

void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg); //서버에서 accept후 생성한 파일디스크립터의 정보를 저장
	int read_cnt;//파일에서 읽은 바이트 수를 저장하기 위한 변수
	int block_cnt = 0; //해당 스레드가 몇 번째 블락을 읽을지에 대한 카운트값을 저장하는 변수
	
	while(1)
	{
		if( block[block_cnt].buf == NULL ) //현재 읽고자 하는 블락이 메모리에 올라와있지 않은지 여부 확인
		{
			/*
			파일에서 읽는 부분은 락을 걸어줘야한다.
			해당 블락이 비어있으므로 ,블락을 채우고있다. 락을 걸지 않으면 채우는 도중에 다른 스레드에서 이블락을 읽으면
			덜 읽힌 데이터를 일게되므로 문제가 발생하기 때문이다.
			레퍼런스 카운팅 또한 동시에 다수의 스레드가 접근하면 해당값의 정확성을 보장할 수 없기때문에 락을 걸어준다.
			*/
			pthread_mutex_lock(&mutx); 
			
			FILE * fp;
			fp = fopen("abc.avi","rb");
			//int flag = fcntl(fp, F_GETFL, 0);
			//fcntl(fp, F_SETFL, flag | O_NONBLOCK);
			
			//현재 읽고자 하는 블락이 메모리에 없다면 파일 포인터를 이용하여 메모리에 올려야 한다.
			fseek(fp, BLOCK_SIZE * block_cnt, SEEK_SET);//파일 포인터를 해당 블록의 시작점으로 이동
			
			block[block_cnt].buf = (char *)malloc(sizeof(char) * BLOCK_SIZE);//블락의 블락사이즈 만큼을 동적으로 힙에 할당
			memset(block[block_cnt].buf,0,BLOCK_SIZE);//블락의 데이터를 담을 버퍼를 초기화
			
			read_cnt = fread((void*)(block[block_cnt].buf),1,BLOCK_SIZE,fp); //파일포인터를 이용하여 해당 블락의 데이터를 읽음
			block[block_cnt].buf_length = read_cnt; //읽은 블락의 바이트 수를 저장
			++block[block_cnt].ref;//해당 블락을 참조하고 있으므로, 참조 카운팅 1 증가
			fclose(fp);
			pthread_mutex_unlock(&mutx);//락의 해제
			/////////////
			write(clnt_sock,block[block_cnt].buf,read_cnt);//받은 만큼을 스트림 소켓을 이용하여 쓴다.
		}
		else
		{
			write(clnt_sock,block[block_cnt].buf,block[block_cnt].buf_length); //보내고자 할 파일 블락이 메모리에 올라와있다면 그냥 그 블락의 버퍼값을 보내줌
		}
		
		/*
		위와 마찬가지로, 레퍼런스 카운팅에서 다수의 스레드가 동시에 접근하게 된다면 해당값의 정확성을 보장할 수 없기 때문에 락을 걸어준다.
		그리고 해당 블락을 참조하는 카운팅 수가 0 이면 더이상 메모리에 있을 필요가 없으므로 동적 메모리를 해제시켜 준다.
		이부분도 락을 걸어야 하는데 락을 걸지 않으면 해제 하는 도중에 그 블락을 읽을 수 있는 등의 문제가 발생할 수 있기 때문이다.
		*/
		pthread_mutex_lock(&mutx); 
		--block[block_cnt].ref;
		if( block[block_cnt].ref <= 0)//참조카운팅이 0이하면 해당 블락은 메모리에서 내려버린다.
		{
			free(block[block_cnt].buf);
			block[block_cnt].buf = NULL;
		}
		pthread_mutex_unlock(&mutx); 
		++block_cnt;//다음에 참조할 블락카운트 갱신
		if( block_cnt > end_block_count-1 )//모든 블락을 다 읽었다면 해당 스레드를 종료하기위한 break 조건
			break;
	}
	
	close(clnt_sock); //모든 전송이 끝났으므로 해당하는 소켓정보를 닫음
	return NULL;//스레드에서 리턴값이 없으므로 NULL을 리턴
} 