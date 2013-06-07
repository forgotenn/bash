#include <poll.h>
#include <stropts.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
typedef struct pollfd pollfd;

pollfd fds[2];
int timeout_msecs = 500;
int ret;

char buf[100];

int main()
{
    fds[0].fd = open("/dev/urandom", O_RDONLY);
    fds[1].fd = 1;

    fds[0].events  = POLLIN;
    fds[1].events = POLLOUT;
    int tmp = read(fds[0].fd, buf, 100);
    printf("%s %d\n", "read", tmp);
    tmp = write(fds[1].fd, buf, 100);  
    return 0;
}
