#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <sys/ipc.h>
#include <string>
#include <signal.h>

using namespace std;

#define PORT 8080

char buff[1024];
static int counter = 0;
int new_socket = 0;
int tcp_socket = 0;
int fd = 0;
bool closed = 0;


void server ()
{
    int optval = 1;

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    inet_aton("127.0.0.1",(in_addr*)&server.sin_addr.s_addr);
    
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    perror("socket");

    int s_sockopt = setsockopt(tcp_socket, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval));
    perror("setsockopt");

    int s_bind = bind(tcp_socket, (struct sockaddr*)&server, sizeof(server));
    perror("bind");

    int lsn = listen(tcp_socket, 10);  
    perror("listen");

    //fcntl(tcp_socket, F_SETFL, fcntl(tcp_socket, F_GETFL, NULL) | SOCK_NONBLOCK); 
    //perror("fcntl");

    struct sockaddr_in client;
    socklen_t sizeclient = sizeof(client);

    while (closed != 1)
    {
        new_socket = accept(tcp_socket, (sockaddr*)&client, &sizeclient); 
        if (new_socket == - 1)
        {
            perror("accept");
            continue;
        }
        printf("connection success\n");
        string str = "filename_"+to_string(counter);
        fd = open(str.c_str(), O_WRONLY | O_NONBLOCK, S_IRWXU);
        if (fd == -1)
        {
            fd = open(str.c_str(), O_RDWR | O_CREAT, S_IRWXU);
            if (fd == -1)
            {
                printf("file create error\n");
                continue;
            }
            else
            {
                printf("file create OK\n");
                errno = 0;
            }
        }
        else
            printf("file open OK\n");
        
        recv(new_socket , &buff, sizeof(buff), 0);
        perror("recv");

        write(fd,(const void *)buff,sizeof(buff));
        perror("write");
        close(fd);
        counter++;
    };

    close(new_socket);
    close(tcp_socket);
}

void sig_handler(int signo)
{
    printf("get SIGPUP or SIGTERM\n");
    closed = 1;
    close(new_socket);
    close(tcp_socket);
    close(fd);
    return;
};

int main(int argc, char* argv[])
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        printf("Fork error\n");
        exit(0);
    }
    else if (pid > 0)
    {
        printf("Daemon already exist\n");
        exit(0);
    }
    setsid();
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);
    server();

    return 0;
}