#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void ErrorHandling(char *message);
#define BUF_SIZE 1024
#define HEADER_SIZE 16
#define DATA_SIZE 1008

int main(int argc, char *argv[]){
	int sd;
	FILE *fp;
	char buf[BUF_SIZE], init[BUF_SIZE], header[BUF_SIZE], data[DATA_SIZE], chk_send[BUF_SIZE],chk_recv[BUF_SIZE];
	int read_cnt, send_str_len, recv_str_len;
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
	
	strcpy( init, "init" ); //udp에서 init을 보내 서버가 보내줄 위치를 알리기 위한 init 패킷
	//str_len = write(sd, init, BUF_SIZE);
	
	while(1)
	{
		send_str_len = sendto( sd, init, BUF_SIZE,0,(struct sockaddr*)&serv_adr,sizeof(serv_adr) );
		
		if( send_str_len == -1 )
		{
			printf("init패킷을 전송하지 못함(재전송요망)\n");
			continue;
		}
		else
		{
			printf("init 패킷 전송을 성공. 이제부터 데이터 주고받자\n");
			break;
		}
	}
	
	
	int order = 0;
	int client_seq = 1, server_seq=0;
	
	while(1)
	{	
		while(1)//server에서 보내는 데이터를 받음
		{
			memset(header, 0, sizeof(header));
			memset(data, 0, sizeof(data));
			memset(buf, 0, sizeof(buf));
			
			adr_sz = sizeof(from_adr);
			read_cnt = recvfrom(sd,buf,BUF_SIZE,0,(struct sockaddr*)&from_adr,&adr_sz);
			//아에 패킷 받는것 조차 실패했을 때는 다시 보내달라고 요청
			
			if( read_cnt == -1 )
			{
				printf("client에서 패킷을 받지 못함( 재전송 요망)\n");
				continue;
			}
			else
			{
				//memset(chk_recv,0,sizeof(chk_recv));
				strcpy(chk_recv, buf);
				//buf 출력
				printf("Buf 내용물 출력 \n");
				for( int i=0; i<BUF_SIZE; ++i )
				{
					printf("%02x",buf[i]);
				}
				printf("\n\n");
				break;	
			}
		}
		
		for(int i=0; i<HEADER_SIZE; ++i ) //헤더분리
		{
			header[i] = buf[i];
		}
		for( int i=HEADER_SIZE; i<BUF_SIZE; ++i) //페이로드 분리
		{
			data[i-HEADER_SIZE]=buf[i];
		}
			
		server_seq = atoi(header); //server에서 보낸 seq
			
		if( server_seq != client_seq ) //클라이언트에서 받을 시작 위치와 서버에서 보낸 위치가 맞지 않으면 재전송 요망
		{
			printf("client에서 패킷 받는 순서가 바뀌었음( 재전송 요망)\n");			memset( init, 0, sizeof(init) );
			strcpy( init, "resend" );

			while(1)
			{	
				send_str_len = sendto(sd, init, BUF_SIZE,0,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
				if( send_str_len == -1 )
				{
					printf("resend 요청 실패, 다시 전송\n");
					continue;
				}
				else
				{
					printf("resend 요청 성공\n");
					break;	
				}
			}
			continue;
		}
	
		
		/////////////////////////////////
		//순서도 맞고, 패킷도 잘 왔으면 파일에 써라
		fwrite((void*)data,1,read_cnt-HEADER_SIZE,fp);
	
		client_seq = server_seq + read_cnt - HEADER_SIZE - 1;//여기까지 받았고.
	
		printf("client에서 %d~%d 까지 받음\n", server_seq, client_seq);//여기까지 받았고
		for( int i=0; i<BUF_SIZE; ++i )
		{
			//printf("%02x",chk_recv[i]);
		}
		printf("\n\n\n");
		++client_seq; // 다음 받을 데이터 시퀀스
		
		memset(init,0,sizeof(init));
		sprintf(init, "%d", client_seq);   //잘받았다는 사인과 함께, 다음번 받을 seq전달

		while(1)
		{
			send_str_len = sendto(sd, init, BUF_SIZE,0,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
			if( send_str_len == -1 )
			{
				printf("ack send error from client\n");
				continue;
			}
			else
			{
				printf("ack 를 보냈음.(client->server)");
				break;
			}
		}
		
		if( read_cnt < BUF_SIZE)
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