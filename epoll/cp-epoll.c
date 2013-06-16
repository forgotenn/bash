#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

const int MAX_BUFSIZE = 100;

typedef struct epoll_event epoll_event;

char** buf;
int* buf_size;
int epfd;
epoll_event* events;

int main(int argc, char** argv)
{
    int n = (argc - 1) / 2;
    buf_size = malloc(n * sizeof(int));
    if (!buf_size)
        perror("malloc buf_size");
    buf = malloc(n);
    if (!buf)
        perror("malloc buf");
    epfd = epoll_create(n);
    if (epfd < 0)
        perror("epoll_create");
    epoll_event cur;
    int ret;
    int i;
    for (i = 0; i < 2 * n; i++)
    {
        buf[i] = malloc(MAX_BUFSIZE);
        if (!buf[i])
            perror("malloc buf");
        buf_size[i] = 0;
        cur.data.fd = atoi(argv[i + 1]);
        printf("fd = %d\n", cur.data.fd);
        cur.events = i % 2 == 0 ? EPOLLIN : EPOLLOUT;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cur.data.fd, &cur);
        if (ret)
            perror("epoll_ctl");
    }

    events = malloc(sizeof(epoll_event) * n);
    if (!events)
        perror("malloc");

    while(1)
    {
        int nr_events = epoll_wait(epfd, events, n, -1);
        printf("nr_events %d\n", nr_events);
        if (nr_events < 0)
        {
            perror("epoll_wait");
            free(events);
            return 1;
        }

        for (i = 0; i < nr_events; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                if (buf_size[i] < MAX_BUFSIZE)
                {
                    int tmp = read(events[i].data.fd, buf[i] + buf_size[i], MAX_BUFSIZE - buf_size[i]);
                    if (tmp == 0)
                    { 
                        printf("Switch off %d\n", events[i].data.fd);
                        events[i].events &=~ EPOLLIN;
                    }
                    if (tmp < 0)
                        perror("read");
                    else 
                        buf_size[i] += tmp;
                }
            } else
                if (events[i].events & EPOLLOUT)
                {
                    int tmp = write(events[i].data.fd, buf[i - 1], buf_size[i - 1]);
                    if (tmp < 0)
                        perror("write");
                    else
                    {
                        memmove(buf[i - 1] + tmp, buf[i  - 1], buf_size[i - 1] - tmp);
                        buf_size[i - 1] -= tmp;
                    }
                }   
            printf("event=%d on fd=%d\n", events[i].events, events[i].data.fd);
        }
    }
    free(events);

    return 0;
}
