#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

const int MAX_NUMBER_FD = 2;
const int MAX_EVENTS = 64;
const int MAX_BUFSIZE = 100;
typedef struct epoll_event epoll_event;
int main()
{
    int epfd = epoll_create(MAX_NUMBER_FD);
    if (epfd < 0)
        perror("epoll create");
    epoll_event in, out;
    in.data.fd = 0;
    out.data.fd = 1;
    in.events = EPOLLIN;
    out.events = EPOLLOUT;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, in.data.fd, &in);
    if (ret)
        perror("epoll_ctl_in");
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, out.data.fd, &out);
    if (ret)
        perror("epoll_ctl_out");
    epoll_event *events = malloc(sizeof(epoll_event) * MAX_EVENTS);
    if (!events)
    {
        perror("malloc");
        return -1;
    }
    int nr_events = epoll_wait(epfd, events, MAX_EVENTS, -1);
    if (nr_events < 0)
    {
        perror("epoll_wait");
        free(events);
        return 1;
    }
    char* buf;
    int bufsize = 0;
    buf = malloc(MAX_BUFSIZE);
    int i;
    for (i = 0; i < nr_events; i++)
    {
        if (events[i].events & EPOLLIN)
        {
           if (bufsize < MAX_BUFSIZE)
           {
               int tmp = read(events[i].data.fd, buf + bufsize, MAX_BUFSIZE - bufsize);
               if (tmp < 0)
                   perror("read");
               else 
                   bufsize += tmp;
            }
        } else
        if (events[i].events & EPOLLOUT)
        {
            int tmp = write(events[i].data.fd, buf, bufsize);
            if (tmp < 0)
                perror("write");
            else
            {
                memmove(buf + tmp, buf, bufsize - tmp);
                bufsize -= tmp;
            }
        }   
        printf("event=%d on fd=%d\n", events[i].events, events[i].data.fd);
    }

    free(events);

    return 0;
}
