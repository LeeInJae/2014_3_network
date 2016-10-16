#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
void ErrorHandling(char *message);
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
	int sd;
	FILE *fp;
	char buf[BUF_SIZE], init[BUF_SIZE];
	int read_cnt, str_len;
	struct sockaddr_in serv_adr, from_adr;
	socklen_t adr_sz;
	
	if(argc!=3){
		printf("Usage: %s <IP><port>\n",argv[0]);
		exit(1);
	}
	
	fp = fopen("receive_udp.pdf","wb");
	
	sd =socket(PF_INET,SOCK_DGRAM,0);
	if(sd==-1)
		ErrorHandling("socket() error");

	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	//connect( sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	
	strcpy( init, "init" );
	//str_len = write(sd, init, BUF_SIZE);
	
	str_len = sendto(sd, init, BUF_SIZE,0,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
	
	
	
	//adr_sz = sizeof(from_adr);
	puts("send init request from client");
	
	int order = 0;
	//while((read_cnt = recvfrom(sd,buf,BUF_SIZE,0,(struct sockaddr*)&from_adr,&adr_sz))!=0)
	while(1)
	{	
		adr_sz = sizeof(from_adr);	
		read_cnt = recvfrom(sd,buf,BUF_SIZE,0,(struct sockaddr*)&from_adr,&adr_sz);
		//read_cnt = read(sd,buf,BUF_SIZE);
		fwrite((void*)buf,1,read_cnt,fp);
		++order;
		printf("%d 받는 단위 부분 Client\n", order);
		if( read_cnt< BUF_SIZE)
			break;
	}	
	//puts("Recieved file data");
	//sendto(sd,"thank you",10,0,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
	printf("client end\n");
	fclose(fp);
	close(sd);
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}