#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 1024
void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}

void * handle_clnt(void * arg);

int clnt_cnt = 0;
int clnt_socks[256];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
	int serv_sd, clnt_sd;
	
	
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	
	socklen_t clnt_adr_sz;
	pthread_t t_id;
	
	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	//FILE * fp;
	//fp = fopen("lec11.pdf","rb");
	
	pthread_mutex_init(&mutx,NULL);
	serv_sd = socket( PF_INET, SOCK_STREAM, 0 );
	
	if(serv_sd==-1)
		error_handling("socket() error");

	memset(&serv_adr,0,sizeof(serv_adr));
	
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); //소켓을 사용하는 컴퓨터의 ip주소 자동 할당
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sd,(struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sd,5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sd=accept(serv_sd,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		
		if(clnt_sd==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n",clnt_cnt+1);
				
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sd;
		pthread_mutex_unlock(&mutx);
		
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sd);
		pthread_detach(t_id);
		//printf("Connected client IP: %s\n",inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sd);
	return 0;
}

void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len = 0, i;
	
	FILE * fp;
	fp = fopen("abc.avi","rb");
	char buf[BUF_SIZE];
	int read_cnt;
	
	pthread_mutex_lock(&mutx);	
	while(1)
	{
		memset(buf,0,sizeof(buf));
		read_cnt = fread((void*)buf,1,BUF_SIZE, fp);
		if( read_cnt < BUF_SIZE){ //보내는 단위가 buf 사이즈보다 작게 남아있으면 그만큼만 읽어들이고 브레이크로 빠져나옴
			write(clnt_sock,buf,read_cnt);
			//printf("FINAL 패킷 \n");
			break;
		}		
		write(clnt_sock,buf,BUF_SIZE);
		//printf("파일 보내자\n");
	}
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	fclose(fp);
	return NULL;
} 