/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char**argv)
{
  int sockfd,send_number,recv_number,n;
  struct sockaddr_in servaddr,cliaddr;
  char sendline[1000000];
  char recvline[1000000];
  char buf[BUFSIZ];
	int count = 0, number1, number2;
  if (argc != 2)
    {
      printf("usage: ex04c <IP address>\n");
      exit(1);
    }

  sockfd=socket(AF_INET,SOCK_DGRAM,0);

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=inet_addr(argv[1]);
  servaddr.sin_port=htons(32000);
	
  while ( 1 )
  {
		switch ( count ) 
		{
			case 0:
			scanf("%d",&send_number);
			      printf( "클라이언트에서 첫번째로 보내는 수는 %d 입니다 \n", send_number );
			      sendto(sockfd, &send_number,sizeof(send_number),0,
			             (struct sockaddr *)&servaddr,sizeof(servaddr));
			      n=recvfrom(sockfd, &recv_number,sizeof(recv_number),0,NULL,NULL);
					printf( "서버측에서 받았다는 첫번째 수는 %d 입니다 \n", recv_number );
					++count;
					number1 = send_number;
					break;
			
			case 1:
			scanf("%d",&send_number);
				 printf( "클라이언트에서 두번째로 보내는 수는 %d 입니다 \n", send_number );
			    sendto(sockfd, &send_number,sizeof(send_number),0,
			    (struct sockaddr *)&servaddr,sizeof(servaddr));
		      n=recvfrom(sockfd, &recv_number,sizeof(recv_number),0,NULL,NULL);
				printf( "서버측에서 계산한 첫번째 수 + 두번째 수는 %d 입니다 \n", recv_number );
				number2 = send_number;
				
				if( number1 + number2 == recv_number )
				{
					printf("서버측에서 계산한답과, 클라이언트에서 계산한답이 같으므로 접속 종료\n");
					close(sockfd);
					exit(0);
				}
				++count;
				break;

			default:
				break;
		}
		/*
		scanf("%d",&send_number);
      printf( "클라이언트에서 보내는 수는 %d 입니다 \n", send_number );
      sendto(sockfd, &send_number,sizeof(send_number),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
      n=recvfrom(sockfd, &recv_number,sizeof(recv_number),0,NULL,NULL);
		printf( "클라이언트에서 받은 수는 %d 입니다 \n", recv_number );
		*/
  }
}