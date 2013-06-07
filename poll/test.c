#include <poll.h>
#include <stropts.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

typedef struct pollfd pollfd;

pollfd fds[3];
int timeout_msecs = 500;
int ret;

char buf[2][100];

int main()
{
    //fds[0].fd = open("/dev/urandom", O_RDONLY);
    //if (fds[0].fd < 0)
    //    perror("open urandom");
    fds[0].fd = 0;
    fds[1].fd = 0;
    fds[2].fd = 1;

    fds[0].events  = POLLIN;
    fds[1].events = POLLIN;
    fds[2].events = POLLOUT;
    int k;
    //for (k = 0; k < 10; k++)
    while(1)
    {
        ret = poll(fds, 3, timeout_msecs);
        if (ret < 0)
            perror("poll");
        printf("Poll\n");
        int buf_size[2];
        buf_size[0] = 0;
        buf_size[1] = 0;     
        if (ret > 0)
        {
            int i; 
            for (i = 0; i < 3; i++)
            {
                if (fds[i].revents & POLLIN) 
                {
                    return 0;
                    printf("%d %d\n", i, buf_size[i]);
                    if (buf_size[i] < 100)
                    {    
                        int tmp  = read(fds[i].fd, buf[i] + buf_size[i], 100 - buf_size[i]);
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
                            }
                    }
                }
                if (fds[i].revents & POLLOUT)
                {
                    int j;
                    for (j = 0; j < 2; j++) 
                    {
                        int tmp = write(fds[i].fd, buf[j], buf_size[j]);
                        if (tmp < 0)
                            perror("write");
                        memmove(buf[j] + tmp, buf[j], buf_size[j] - tmp);
                    }

                }
            }    
        }
    }
    return 0;
}
