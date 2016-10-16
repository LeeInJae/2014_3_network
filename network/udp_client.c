/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char**argv)
{
  int sockfd,n;
  struct sockaddr_in servaddr,cliaddr;
  char sendline[1000000];
  char recvline[1000000];
  char buf[BUFSIZ];

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
	
  while (fgets(buf, BUFSIZ,stdin) != NULL)
    {
      int i,n;
      n = atoi(buf) - 1;
      for(i = 0; i< n; i++) {
	sendline[i] = 'a' + i % 26;
      }
      sendline[i] = '\n';
      i++;
      sendline[i] = 0;
      sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
      n=recvfrom(sockfd,recvline,1000000,0,NULL,NULL);
      recvline[n]=0;
      fputs(recvline,stdout);
    }
}