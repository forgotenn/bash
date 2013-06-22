#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string>
#include <vector>
#include <map>
#include <string.h>

int const MAX_EVENTS_SIZE = 100;
const int MAX_BUFSIZE = 100;

using std::string;
using std::vector;
using std::map;

int epfd;
int* fd;
epoll_event events[MAX_EVENTS_SIZE];

map<string, int> client_ids;

int listenfd,connfd,n;
char recvline[1000];
struct sockaddr_in servaddr,cliaddr;
socklen_t clilen;
pid_t childpid;
char mesg[1000];

bool is_chain = false;
int first = -1;
int last = -1;

bool check(int id);
void prepare();

struct client
{
    int id;
    int fd;
    string prev, next;
    string name;
    char buf[MAX_BUFSIZE];
    int buf_size;
    int flag; // 0 - name, 1 - prev, 2 - next 
    client(int id_, int fd_): id(id_), fd(fd_), buf_size(0), flag()
    {};

    void work_read_prepipe()
    {
        int read_bytes = read(fd, buf + buf_size, MAX_BUFSIZE - buf_size);
        buf_size += read_bytes;
        get_name();
        //printf("get_name done!\n");
        if (flag == 3)
        {
            printf("Client_fd=%d, name=%s, prev=%s, next=%s\n", fd, name.c_str(), prev.c_str(), next.c_str());
            epoll_event cur;
            cur.events = 0;
            cur.data.fd = -id;
            int ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &cur);
            if (ret < 0)
                perror("epoll_ctl");
            client_ids[name] = id;
            is_chain = check(id);
        }
        if (is_chain)
            prepare();
    }

    void work_read_pipe()
    {
    }
    void work_read()
    {
        if (!is_chain)
            work_read_prepipe();
        else 
            work_read_pipe();
    }

    void get_name()
    {
        // printf("get_name\n");
        if (flag == 3)
            return;
        int i = 0;
        while ((i < buf_size) && (buf[i] != '\n'))
        {
            if (flag == 0)
                name += buf[i];
            else if (flag == 1)
                prev += buf[i];
            else if (flag == 2)
                next += buf[i];
            i++;
        }
        if (buf[i] == '\n')
        {
            i++;        
            flag++;
        }

        memmove(buf, buf + i, buf_size - i);
        buf_size -= i;
    }
};


vector<client> clients;
bool check(int id)
{
    string cur_name = clients[id].name;
    while (first == -1)
    {
        if (client_ids.find(cur_name) != client_ids.end())
        {
            if (clients[client_ids[cur_name]].prev == "none")
                first = client_ids[cur_name];
            else
                cur_name = clients[client_ids[cur_name]].prev;
        } else
            return false;
    }
    cur_name = clients[id].name;
    while (last == -1)
    {
        if (client_ids.find(cur_name) != client_ids.end())
        {
            if (clients[client_ids[cur_name]].next == "none")
                last = client_ids[cur_name];
            else
                cur_name = clients[client_ids[cur_name]].next;
        } else
            return false;
    }
    return true;
}

void prepare()
{
    printf("first=%s, last=%s\n", clients[first].name.c_str(), clients[last].name.c_str());
    printf("Starting pipe\n");
    int cur_client = first;
    epoll_event cur;
    int ret;
    while (true)
    {
        int next_client = client_ids[clients[cur_client].next];
        if (clients[cur_client].buf_size < MAX_BUFSIZE)
        {
            cur.events = EPOLLIN;
            cur.data.fd = -cur_client;
            ret = epoll_ctl(epfd, EPOLL_CTL_MOD, clients[cur_client].fd, &cur);
            if (ret < 0)
                perror("epoll_ctl");
        }
        if (cur_client == last)
            break;
        if ((clients[cur_client].buf_size > 0))
        {
            cur.events = EPOLLOUT;
            cur.data.fd = -next_client;
            ret = epoll_ctl(epfd, EPOLL_CTL_MOD, clients[next_client].fd, &cur); 
            if (ret < 0)
                perror("epoll_ctl");
        }
    }
}

int main(int argc, char**argv)
{
    listenfd=socket(AF_INET,SOCK_STREAM,0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    int yes = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
        perror("setsockopt < 0");
        exit(1);
    }
    if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        perror("bind");

    if (listen(listenfd,1024) < 0)
        perror("listen");
    epfd = epoll_create(1);
    epoll_event cur;
    cur.events = EPOLLIN;
    cur.data.fd = listenfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &cur);
    if (ret < 0)
        perror("epoll_ctl");

    for(;;)
    {
        clilen = sizeof(cliaddr);
        int nr_events = epoll_wait(epfd, events, MAX_EVENTS_SIZE, -1);
        if (nr_events < 0)
        {
            perror("epoll_wait");
        }
        for (int i = 0; i < nr_events; i++)
        {
            if ((events[i].data.fd == listenfd) && (events[i].events & EPOLLIN))
            {
                connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
                clients.push_back(client(clients.size(), connfd));
                cur.events = EPOLLIN;
                cur.data.fd = -clients.back().id;//using as not listenfd
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &cur);
                printf("Accept %d\n", connfd);
            } else
                if (events[i].events & EPOLLIN)
                {
                    int id = -events[i].data.fd;
                    clients[id].work_read();
                }
        }
    }
}
