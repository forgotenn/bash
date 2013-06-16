nclude <poll.h>
#include <stropts.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct pollfd pollfd;
const int BUFSIZE = 100;
const int TIMEOUT = 500;
pollfd* fds;
char** buf;
int* buf_size;

int main(int argc, char** argv)
{
    int n = (argc - 1) / 2;
    int i;
    buf_size = malloc(n * sizeof(int));
    buf = malloc(n);
    for (i = 0; i < n; i++)
    {
        buf[i] = malloc(BUFSIZE);
        buf_size[i] = 0;
    }
    fds = malloc(n);
    for (i = 0; i < 2 * n; i++)
    {
        fds[i].fd = atoi(argv[i + 1]);
        fds[i].events = (i % 2 == 0) ? POLLIN : POLLOUT;
    }
    while(1)
    {
        int ret = poll(fds, 2*n, TIMEOUT);
        if (ret < 0)
            perror("poll");
        if (ret > 0)
        {
            for (i = 0; i < 2 * n; i++)
            {
                if (fds[i].revents & POLLIN) 
                {
                    if (buf_size[i] < BUFSIZE)
                    {    
                        int tmp  = read(fds[i].fd, buf[i] + buf_size[i], BUFSIZE - buf_size[i]);
                        if (tmp == 0)
                        {
                            printf("%s %d", "Switch off ", i);
                            fds[i].events &=~ POLLIN; 
                        } else 
                            if (tmp < 0)
                                perror("read");
                            else 
                            {
                                buf_size[i] += tmp;
                                fds[i+1].events |= POLLOUT;
                            }
                    }
                }
                if (fds[i].revents & POLLOUT)
                {
                        int tmp = write(fds[i].fd, buf[i - 1], buf_size[i - 1]);
                        if (tmp < 0)
                            perror("write");
                        memmove(buf[i - 1] + tmp, buf[i - 1], buf_size[i - 1] - tmp);
                        buf_size[i - 1] -= tmp;
                    if (buf_size[i - 1] == 0)
                            fds[i].events &=~ POLLOUT;

                }
            }    
        }
    }
    return 0;
}
