#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}

int main(int argc, char *argv[]){
	int serv_sd, clnt_sd;
	char buf[BUF_SIZE];
	int read_cnt, str_len;
	
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	
	socklen_t clnt_adr_sz;
	FILE * fp;
	
	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	fp = fopen("lec11.pdf","rb");
	serv_sd = socket( PF_INET, SOCK_DGRAM, 0 );
	
	if(serv_sd==-1)
		error_handling("socket() error");

	memset(&serv_adr,0,sizeof(serv_adr));
	
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sd,(struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
		
	clnt_adr_sz=sizeof(clnt_adr);
	str_len = recvfrom(serv_sd, buf, BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
	printf("str_len : %d,     INIT HOST orderd port: %#x\n",str_len,clnt_adr.sin_port);
	
	if( str_len != -1)
	{
		printf("Recieve from init request(server) \n");
	}
	else
	{
		printf("init fail")	;
	}
	
	int order = 0;
	
	while(1)
	{
		read_cnt = fread((void*)buf,1,BUF_SIZE, fp);
		
		if( read_cnt < BUF_SIZE){ //보내는 단위가 buf 사이즈보다 작게 남아있으면 그만큼만 읽어들이고 브레이크로 빠져나옴
			sendto(serv_sd, buf, read_cnt, 0,(struct sockaddr*)&clnt_adr,clnt_adr_sz);
			printf("마지막 부분\n");
			break;
		}		
		sendto(serv_sd, buf, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
		++order;
		printf(" %d 보내는 단위 부분 Server, HOST orderd port: %#x\n", 		order,clnt_adr.sin_port);
		
	}
	
	//shutdown(clnt_sd,SHUT_WR);
	//recv(clnt_sd,buf,BUF_SIZE);
	//recvfrom(serv_sd,buf,BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
	//printf("Message from client:%s \n",buf);
	
	printf(" server end\n");
	fclose(fp);
	close(serv_sd);
	return 0;
}