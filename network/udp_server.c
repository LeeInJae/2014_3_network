/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char**argv)
{
  int sockfd,n;
  struct sockaddr_in servaddr,cliaddr;
  socklen_t len;
  char mesg[1000000];
  char buf[100];

  sockfd=socket(AF_INET,SOCK_DGRAM,0);

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(32000);
  bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

  for (;;)
    {
      len = sizeof(cliaddr);
      n = recvfrom(sockfd,mesg,1000000,0,(struct sockaddr *)&cliaddr,&len);
      strcpy(buf,inet_ntoa(cliaddr.sin_addr));
      sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      printf("-------------------------------------------------------\n");
      mesg[n] = 0;
      printf("Received the following %d bytes from %s:%d:\n", n, buf, ntohs(cliaddr.sin_port));
      printf("%s",mesg);
      printf("-------------------------------------------------------\n");
    }
}