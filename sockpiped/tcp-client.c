#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char**argv)
{
   int sockfd;
   struct sockaddr_in servaddr;
   char sendline[1000];

   sockfd=socket(AF_INET,SOCK_STREAM,0);

   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(argv[1]);
   servaddr.sin_port=htons(32000);

   connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

   while (fgets(sendline, 10000,stdin) != NULL)
   {
      sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
   }
   return 0;
}
