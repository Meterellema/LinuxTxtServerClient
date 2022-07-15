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

#define PORT 8080

char buff[1024];


int main(int argc, char* argv[])
{
    char path[512];
    int fd=0;

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    inet_aton("127.0.0.1",(in_addr*)&server.sin_addr.s_addr);

    while(true)
    {
        printf("Enter path to file or filename\n");
        scanf ("%s",path);
        fd = open(path, O_RDONLY | O_NONBLOCK, S_IRWXU);
        if (fd == -1)
            perror("open");
        else
        {
            printf("file open\n");
            break;
        }
    }

    read(fd, &buff, sizeof(buff));
    perror("read");

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    perror("socket");

    int cnct=1;

    while(cnct != 0)
    {   
        cnct = connect(client_socket, (sockaddr*)&server, sizeof(server));
        perror("connect");
        sleep(1);
    }; 
    printf("Send msg: %s\n", buff);

    send(client_socket, &buff, sizeof(buff), 0);

    close(client_socket);
    close(fd);


    return 0;
}