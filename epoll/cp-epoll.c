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
int* fd;
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
    fd = malloc(2 * n * sizeof(int));
    if (!fd)
        perror("malloc fd");
    events = malloc(n * sizeof(epoll_event));
    if (!events)
        perror("malloc events");
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
        cur.data.fd = i;
        fd[i] = atoi(argv[i + 1]);
        printf("fd = %d\n", cur.data.fd);
        cur.events = i % 2 == 0 ? EPOLLIN : !EPOLLOUT;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd[cur.data.fd], &cur);
        if (ret)
            perror("epoll_ctl");
    }

    epoll_event event_mod;
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

        int cur_fd_id;
        for (i = 0; i < nr_events; i++)
        {
            cur_fd_id = events[i].data.fd;
            if (events[i].events & EPOLLIN)
            {
                if (buf_size[i] < MAX_BUFSIZE)
                {
                    int tmp = read(fd[cur_fd_id], buf[cur_fd_id] + buf_size[cur_fd_id], MAX_BUFSIZE - buf_size[cur_fd_id]);
                    if (tmp == 0)
                    { 
                        event_mod.events = 0;
                        event_mod.data.fd = fd[cur_fd_id];
                        ret = epoll_ctl(epfd, EPOLL_CTL_MOD, event_mod.data.fd, &event_mod);
                        if (ret)
                            perror("epoll_ctl_!EPOLLIN");
                    }
                    if (tmp < 0)
                        perror("read");
                    else 
                    {
                        buf_size[cur_fd_id] += tmp;
                        event_mod.events = EPOLLOUT;
                        event_mod.data.fd = cur_fd_id + 1;
                        ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd[cur_fd_id + 1], &event_mod);
                        if (ret)
                            perror("epoll_ctl_EPOLLOUT");
                    }
                }
            } else
                if (events[i].events & EPOLLOUT)
                {
                    int tmp = write(fd[cur_fd_id], buf[cur_fd_id - 1], buf_size[cur_fd_id - 1]);
                    if (tmp < 0)
                        perror("write");
                    else
                    {
                        memmove(buf[cur_fd_id - 1] + tmp, buf[cur_fd_id  - 1], buf_size[cur_fd_id - 1] - tmp);
                        buf_size[cur_fd_id - 1] -= tmp;
                        if (buf_size[cur_fd_id - 1] == 0)
                        {
                            event_mod.events = 0;
                            event_mod.data.fd = cur_fd_id;
                            ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd[cur_fd_id], &event_mod);
                            if (ret)
                                perror("epoll_ctl_!EPOLLOUT");
                        }
                        if (buf_size[cur_fd_id - 1] < MAX_BUFSIZE)
                        {
                            event_mod.events = EPOLLIN;
                            event_mod.data.fd = cur_fd_id - 1;
                            ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd[cur_fd_id - 1], &event_mod);
                            if (ret)
                                perror("epoll_ctl_EPOLLIN");
                            
                        }
                    }
                }   
            printf("event=%d on fd=%d\n", events[i].events, fd[events[i].data.fd]);
        }
    }
    free(events);

    return 0;
}
