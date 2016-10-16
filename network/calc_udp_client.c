/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char**argv)
{
  int sockfd,n;
  struct sockaddr_in servaddr,cliaddr;
  char sendline[1000000];
  char recvline[1000000];
  char buf[BUFSIZ];

  if (argc != 2) //ip주소 입력받아라
    {
      printf("usage: ex04c <IP address>\n");
      exit(1);
    }

  sockfd=socket(AF_INET,SOCK_DGRAM,0);

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=inet_addr(argv[1]);
  servaddr.sin_port=htons(32000);

  int count = 0;
  int number1, number2, sum, server_sum;
  
  while (fgets(buf, BUFSIZ,stdin) != NULL)
    {
		strcpy( sendline, buf );
      ++count;
		if( count == 1 ) { number1 = atoi( buf );}
		else if( count == 2 ) { number2 = atoi( buf ); }
		
      sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
      n=recvfrom(sockfd,recvline,1000000,0,NULL,NULL);
		
		
      recvline[n]=0;
      fputs(recvline,stdout);
		
		
		if( count >= 2)
		{
			sum = number1 + number2;
			server_sum = atoi( recvline );
			if ( sum == server_sum )
			{
				close(sockfd);	
				exit(0);
			}
		}
    }
}