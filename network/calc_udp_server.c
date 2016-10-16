/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char**argv)
{
  int sockfd,n;
  struct sockaddr_in servaddr,cliaddr;
  socklen_t len;
  char mesg[1000000];
  char buf[100];
  char temp[100];
  sockfd=socket(AF_INET,SOCK_DGRAM,0);

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(32000);
  bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	
	int count = 0;
	int number1 = 0, number2 = 0;
	int mesg_len = 0;
	
  for ( ;; )
    {
      len = sizeof(cliaddr);
      
		int check = 1;
		int number = 0;
		
		n = recvfrom(sockfd,mesg,1000000,0,(struct sockaddr *)&cliaddr,&len);

		for( int i = 0; i < n - 1; ++i )
		{
			if( (int)mesg[ i ] <'0' || (int)mesg[ i ] > '9' )
			{
				strcpy( mesg, "숫자가 아닙니다 다시 입력해 주십시오\n");
				check = 0;
				break;
			}
		}
		
		if( check )
		{
			++count;
			sprintf( temp,"%d 번째의 숫자가 입력되었습니다\n", count );
			if( count == 1)
			{
				number1 = atoi( mesg );
			}
			else if ( count == 2 )
			{
				number2 = atoi( mesg );
				sprintf( temp,"%d 번째의 숫자가 입력되었습니다. 두 수의 합을 보려면 엔터를 눌러주세요\n", count );
			}
			
			strcpy( mesg, temp );
		}
		
		if( count < 2 )
		{
		 mesg_len = strlen( mesg );
      strcpy(buf,inet_ntoa(cliaddr.sin_addr));
      sendto(sockfd,mesg,mesg_len,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      printf("-------------------------------------------------------\n");
      mesg[mesg_len] = 0;
      printf("Received the following %d bytes from %s:%d:\n", n, buf, ntohs(cliaddr.sin_port));
      printf("%s",mesg);
      printf("-------------------------------------------------------\n");
		}
		if( count >= 2 )
		{
				sprintf( temp,"%d\n", number1 + number2 );
				 strcpy( mesg, temp );
				 mesg_len = strlen( mesg );
				 mesg[mesg_len] = 0;
			    sendto(sockfd,mesg,mesg_len,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
			    printf("-------------------------------------------------------\n");
			   // mesg[mesg_len] = 0;
			    printf("Received the following %d bytes from %s:%d:\n", n, buf, ntohs(cliaddr.sin_port));      				 printf("%s",mesg);
			    printf("-------------------------------------------------------\n");
		}
    }
}