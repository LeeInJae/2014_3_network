#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define HEADER_SIZE 16
#define DATA_SIZE 1008

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}

int main(int argc, char *argv[]){
	int serv_sd, clnt_sd;
	char buf[BUF_SIZE], temp[DATA_SIZE],header[BUF_SIZE], data[DATA_SIZE], init[DATA_SIZE], chk_send[BUF_SIZE],chk_recv[BUF_SIZE];
	int read_cnt, send_str_len,recv_str_len;
	
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
		
	//while(1)
	//{
		clnt_adr_sz=sizeof(clnt_adr);
		recv_str_len = recvfrom(serv_sd, buf, BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		if( !strcmp(buf,"init") )
		{
			printf("Recieve from init request(server) \n");
			//break;
		}
		else 
		{
			printf("init fail");
		}
	//}
	
	//printf("str_len : %d,     INIT HOST orderd port: %#x\n",str_len,clnt_adr.sin_port);
	
	int order = 0;
	int server_seq = 1, client_seq = 0;
	
	while(1)
	{
		memset(buf,0,sizeof(buf));
		memset(header,0,sizeof(header));
		memset(data,0,sizeof(data));
		memset(temp,0,sizeof(temp));
		
		read_cnt = fread((void*)temp,1,DATA_SIZE, fp);
		printf("read_cnt  = %d      ,        DATA_SZE = %d \n",read_cnt, DATA_SIZE );
		sprintf( buf, "%d", server_seq );
		
		if( read_cnt < DATA_SIZE )
		{ 
			for( int i=HEADER_SIZE; i<HEADER_SIZE+read_cnt; ++i )
			{
				buf[i] = temp[i-HEADER_SIZE];
			}
	
			while(1)
			{
				while(1)
				{
					send_str_len = sendto(serv_sd, buf, HEADER_SIZE+read_cnt, 0,(struct sockaddr*)&clnt_adr,clnt_adr_sz);
					if( send_str_len == -1 )
					{
						printf("서버에서 패킷을 전송하지 못했음 \n");
						continue;
					}
					else 
						break;
				}
				
				while(1)
				{
					recv_str_len = recvfrom(serv_sd, buf, BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
					if( recv_str_len == -1 )
					{
						printf( "client로부터 seq못받음 \n");
						continue;
					}
					else
						break;
				}
				
				if( !strcmp(buf,"resend") )
				{
					continue;
				}
				else //잘 받았으면
				{	
					client_seq = atoi(buf);
					printf("마지막 부분 %d에서부터 %d까지 패킷을 전송 \n",server_seq, client_seq -1 );
					server_seq = client_seq;
					break;
				}
			}
			//전송 끝
			break;
		}
		
		
		for( int i=HEADER_SIZE; i<HEADER_SIZE+DATA_SIZE; ++i )
		{
			buf[ i ] = temp[ i-HEADER_SIZE ];
		}
		
		/*
		printf(" BUF 내용물 출력 \n");
		for( int i=0; i<BUF_SIZE; ++i )
		{
			printf("%02x",buf[i]);
		}
		printf("\n");
		*/
		
		while(1)
		{
			while(1)
			{
				send_str_len = sendto(serv_sd, buf, BUF_SIZE, 0,(struct sockaddr*)&clnt_adr,clnt_adr_sz);
				if( send_str_len == -1 )
				{
					printf("서버에서 패킷을 전송하지 못했음 \n");
					continue;
				}
				else 
				{
					//memset(chk_send,0,sizeof(chk_send));
					printf("client로 패킷 전송 잘했음\n");
					strcpy(chk_send, buf);
					break;
				}
			}
			
			while(1)
			{
				recv_str_len = recvfrom(serv_sd, buf, BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
				if( recv_str_len == -1 )
				{
					printf( "client로부터 seq못받음 \n");
					continue;
				}
				else
				{
					printf("client로부터 seq잘 받음\n");
					strcpy(chk_recv, buf);
					break;
				}
			}
			
			if( !strcmp(buf,"resend") )
			{
				printf(" resend 해주세요 from server \n");
				continue;
			}
			else //잘 받았으면
			{	
				client_seq = atoi(buf);
				printf("%d 에서부터 %d까지 패킷을 전송 \n",server_seq, client_seq -1 );
				for( int i=0; i<BUF_SIZE; ++i )
				{
				//	printf("%02x",chk_send[i]);
				}
				printf("\n\n\n");
				server_seq = client_seq;
				break;
			}
		}
	}
	
	//shutdown(clnt_sd,SHUT_WR);
	//recv(clnt_sd,buf,BUF_SIZE);
	//recvfrom(serv_sd,buf,BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
	//printf("Message from client:%s \n",buf);
	
	printf("server end\n");
	fclose(fp);
	close(serv_sd);
	return 0;
}