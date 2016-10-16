/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char**argv)
{
  int sockfd, send_number, recv_number,n,sum;
  struct sockaddr_in servaddr,cliaddr;
  socklen_t len;
  char mesg[1000000];
  char buf[100];
	int number1,number2, count = 0;
	
  sockfd=socket(AF_INET,SOCK_DGRAM,0);

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(32000);
  bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	
  for (;;)
    {
		switch( count )
		{
			case 0:
			len = sizeof(cliaddr);
	 	   n = recvfrom(sockfd,&recv_number,sizeof(int),0,(struct sockaddr *)&cliaddr,&len);
			number1 = recv_number;
			
			strcpy(buf,inet_ntoa(cliaddr.sin_addr));
	     sendto(sockfd,&recv_number,sizeof(int),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
		  printf("-------------------------------------------------------\n");
	     printf("Received the following %d bytes from %s:%d:\n", n, buf, ntohs(cliaddr.sin_port)); 	     printf("(서버측에서 첫 번째로 받은 수는 %d 입니다\n", recv_number);		  printf("-------------------------------------------------------\n");
			++count;
			break;
			
			case 1:
			len = sizeof(cliaddr);
				 	   n = recvfrom(sockfd,&recv_number,sizeof(int),0,(struct sockaddr *)&cliaddr,&len);
						number2 = recv_number;
						sum = number1 + number2;
						strcpy(buf,inet_ntoa(cliaddr.sin_addr));
				     sendto(sockfd,&sum,sizeof(int),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					  printf("-------------------------------------------------------\n");
				     printf("Received the following %d bytes from %s:%d:\n", n, buf, ntohs(cliaddr.sin_port)); 	     printf("서버측에서 두 번째로 받은 수는 %d 이고 두 수의 합은 %d 입니다\n", recv_number, sum);		  printf("-------------------------------------------------------\n");
			++count;
						break;
			
			default:
						break;
		}
		
		/*
      len = sizeof(cliaddr);
      n = recvfrom(sockfd,&recv_number,sizeof(recv_number),0,(struct sockaddr *)&cliaddr,&len);
		printf("n = %d\n",n);
		strcpy(buf,inet_ntoa(cliaddr.sin_addr));
      sendto(sockfd,&recv_number,sizeof(recv_number),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      printf("-------------------------------------------------------\n");
      printf("Received the following %d bytes from %s:%d:\n", n, buf, ntohs(cliaddr.sin_port));
      printf("클라이언트에서 받은 수는 %d 입니다", recv_number);
      printf("-------------------------------------------------------\n");
*/
    }
}