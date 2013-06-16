#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_NUMBER_FD = 2;
const int MAX_EVENTS = 64;
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
    int i;
    for (i = 0; i < nr_events; i++)
    {
        printf("event=%d on fd=%d\n", events[i].events, events[i].data.fd);
    }
    free(events);

    return 0;
}
