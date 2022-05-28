#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>

#define BUF_SIZE 1050000
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int client[32] = {0};
int in[32] = {0};

void handle_chat(int data)
{
    int head = 1;
    int flag = 0;
    int big = 0;
    // struct Pipe *pipe = (struct Pipe *)data;
    char buf[BUF_SIZE];
    char buffer[BUF_SIZE];
    ssize_t len;
    char msg[BUF_SIZE] = "Message:";
    int start = 8;
    // scanf("%s",msg);
    // strncpy(buffer + 8 ,msg,BUF_SIZE);
    int i;
    memset(buffer, 0, sizeof(buffer));
    while (1)
    {
        len = recv(client[data], buffer, BUF_SIZE, 0);
        if (len <= 0)
        {
            if (head)
            {
                in[data] = 0;
                close(client[data]);
                return;
            }
            return;
        }
        head = 0;

        int i, j;
        int sig = 0;
        int num = 0;
        for (i = 0; i < len; i++)
        {
            if (buffer[i] == '\n')
            {
                num = i - sig + 1;
                strncpy(msg + start, buffer + sig, num);

                for (j = 0; j < 32; j++)
                {
                    if (in[j] && j != data)
                    {
                        int remain = start + num;
                        int sended = 0;
                        while (remain > 0)
                        {
                            sended = send(client[j], msg + sended, remain, 0);
                            if (sended == -1)
                            {
                                perror("send");
                                exit(-1);
                            }
                            remain -= sended;
                        }
                    }
                }

                sig = i + 1;
                start = 8;
            }
        }
        if (sig != len)
        {
            num = len - sig;
            strncpy(msg + start, buffer + sig, num);
            start = start + num;
        }
    }
    // while (1)
    // {
    //     len = recv(client[data], buffer, BUF_SIZE, 0);
    //     if (len <= 0)
    //     {
    //         if (head)
    //         {
    //             in[data] = 0;
    //             close(client[data]);
    //             return;
    //         }

    //         return;
    //     }
    //     head = 0;
    //     strcpy(buf, "Message:");
    //     for (int j = 0; j < 32; j++)
    //     {
    //         if (in[j] == 0 || j == data)
    //             continue;
    //         for (i = 0; i < BUF_SIZE; i++)
    //         {
    //             if (buffer[i] == '\n')
    //             {
    //                 if (big == 0)
    //                 {
    //                     strcat(buf, buffer);
    //                     send(client[j], buf, i + 9, 0);
    //                     strncpy(buffer, buffer + i + 1, BUF_SIZE);
    //                     i = 0;
    //                     flag = 1; // send
    //                 }
    //                 else
    //                 {
    //                     send(client[j], buffer, i + 1, 0);
    //                     strncpy(buffer, buffer + i + 1, BUF_SIZE);
    //                     i = 0;
    //                     flag = 1; // send
    //                     big = 0;
    //                 }
    //             }
    //         }
    //         if (flag == 0)
    //         {
    //             if (big == 0)
    //             {
    //                 strcat(buf, buffer);
    //                 send(client[j], buf, i + 8, 0);
    //                 // strncpy(buffer+8,buffer+i+1,BUF_SIZE);
    //                 big = 1;
    //             }
    //             else
    //             {
    //                 send(client[j], buffer, i, 0);
    //                 // strncpy(buffer+8,buffer+i+1,BUF_SIZE);
    //             }
    //         }
    //     }

    //     // len < 0 delete pipe
    // }
    return;
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
    if (listen(fd, 32))
    {
        perror("listen");
        return 1;
    }
    // struct Pipe *head = (struct Pipe *)malloc(sizeof(struct Pipe));
    int fdmax = fd;

    int i = 0;

    fd_set clients;

    while (1)
    {

        FD_ZERO(&clients);
        FD_SET(fd, &clients);

        for (i = 0; i < 32; i++)
        {
            if (in[i])
                FD_SET(client[i], &clients);
        }
        if (select(fdmax + 1, &clients, NULL, NULL, NULL) > 0)
        {
            if (FD_ISSET(fd, &clients))
            {
                int fdnew = accept(fd, NULL, NULL);
                if (fdnew == -1)
                {
                    perror("accept");
                    return 1;
                }
                fcntl(fdnew, F_SETFL, fcntl(fdnew, F_GETFL, 0) | O_NONBLOCK);
                if (fdmax < fdnew)
                {
                    fdmax = fdnew;
                }
                for (i = 0; i < 32 && in[i]; i++)
                    ;

                in[i] = 1;
                client[i] = fdnew;
            }
            else
            {
                for (i = 0; i < 32; i++)
                {
                    if (in[i] && FD_ISSET(client[i], &clients))
                    {
                        handle_chat(i);
                    }
                }
            }
        }
    }
    return 0;
}
