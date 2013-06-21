#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

int sockfd;
struct sockaddr_in servaddr;

void openlog(const char* ident, int option, int facility)
{
    printf("123\n");
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(ident);
    servaddr.sin_port=htons(option);
}

void syslog(int priority, const char *format, ...)
{
    printf("123\n");
    char sendline[10000];
    va_list vl;
    va_start(vl, format);
    int len = vsprintf(sendline, format, vl);
    va_end(vl);
    sendto(sockfd,sendline, len, 0,
            (struct sockaddr *)&servaddr,sizeof(servaddr));
}

void closelog(void)
{
    close(sockfd);
}

