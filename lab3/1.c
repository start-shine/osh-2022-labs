#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
struct Pipe
{
    int fd_send;
    int fd_recv;
};

void *handle_chat(void *data)
{
    int flag = 0;
    int big = 0;
    struct Pipe *pipe = (struct Pipe *)data;
    char buf[BUF_SIZE + 10] = "Message:";
    char buffer[BUF_SIZE];
    ssize_t len;
    char msg[BUF_SIZE];
    // scanf("%s",msg);
    // strncpy(buffer + 8 ,msg,BUF_SIZE);
    int i;
    memset(buffer, 0, sizeof(buffer));
    while ((len = recv(pipe->fd_send, buffer, BUF_SIZE, 0)) > 0)
    {
        strcpy(buf, "Message:");
        for (i = 0; i < BUF_SIZE; i++)
        {
            if (buffer[i] == '\n')
            {
                if (big == 0)
                {
                    strcat(buf, buffer);
                    send(pipe->fd_recv, buf, i + 9, 0);
                    strncpy(buffer, buffer + i + 1, BUF_SIZE);
                    i = 0;
                    flag = 1; // send
                }
                else
                {
                    send(pipe->fd_recv, buffer, i + 1, 0);
                    strncpy(buffer, buffer + i + 1, BUF_SIZE);
                    i = 0;
                    flag = 1; // send
                    big = 0;
                }
            }
        }
        if (flag == 0)
        {
            if (big == 0)
            {
                strcat(buf, buffer);
                send(pipe->fd_recv, buf, i + 8, 0);
                // strncpy(buffer+8,buffer+i+1,BUF_SIZE);
                big = 1;
            }
            else
            {
                send(pipe->fd_recv, buffer, i, 0);
                // strncpy(buffer+8,buffer+i+1,BUF_SIZE);
            }
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int port = atoi(argv[1]);
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket");
        return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    socklen_t addr_len = sizeof(addr);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind");
        return 1;
    }
    if (listen(fd, 2))
    {
        perror("listen");
        return 1;
    }
    int fd1 = accept(fd, NULL, NULL);
    int fd2 = accept(fd, NULL, NULL);
    if (fd1 == -1 || fd2 == -1)
    {
        perror("accept");
        return 1;
    }
    pthread_t thread1, thread2;
    struct Pipe pipe1;
    struct Pipe pipe2;
    pipe1.fd_send = fd1;
    pipe1.fd_recv = fd2;
    pipe2.fd_send = fd2;
    pipe2.fd_recv = fd1;
    pthread_create(&thread1, NULL, handle_chat, (void *)&pipe1);
    pthread_create(&thread2, NULL, handle_chat, (void *)&pipe2);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}
