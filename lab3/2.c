#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 1050
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct Pipe
{
    int fd;
    struct Pipe *next;
};

struct Fd
{
    int num;
    struct Fd *next;
};
int client = 0;
struct Pipe *head;
void *handle_chat(void *data)
{
    int flag = 0;
    int big = 0;
    struct Pipe *pipe = (struct Pipe *)data;
    char buf[BUF_SIZE] = "Message:";
    char buffer[BUF_SIZE];
    ssize_t len;
    char msg[BUF_SIZE];
    // scanf("%s",msg);
    // strncpy(buffer + 8 ,msg,BUF_SIZE);
    int i;
    memset(buffer, 0, sizeof(buffer));
    // pthread_mutex_lock(&mutex);
    int index = 0;
    int send_flag = 0;
    int count = 0;
    // int jike = 0;
    // FILE * file = fopen("1_out.txt","w");
    while ((len = recv(pipe->fd, buffer, BUF_SIZE, 0)) > 0)
    {
        strcpy(buf, "Message:");
        // int index = i + 8;
        int last = 0;
        for (i = 0; i < len; i++)
        {
            flag = 0;
            buf[i + 8 - last] = buffer[i];

            if (buffer[i] == '\n')
            {
                // printf("%d ", i + 8 - last);
                struct Pipe *q = pipe->next;
                while (q != pipe)
                {
                    send(q->fd, buf, i + 9 - last, 0);
                    q = q->next;
                }
                flag = 1;
                // strncpy(buffer, buffer + i + 1, BUF_SIZE);
                // i = 0;
                last = i + 1;
                strcpy(buf, "Message:");
            }
        }
        if (flag == 0 && len == BUF_SIZE)
        {
            // printf("%d ", i + 8 - last);
            struct Pipe *q = pipe->next;
            while (q != pipe)
            {
                send(q->fd, buf, i + 8 - last, 0);
                q = q->next;
            }
        }
    }

    // len < 0 delete pipe
    if (pipe == head)
    {
        head = pipe->next;
    }
    struct Pipe *q = pipe;
    while (q->next != pipe)
    {
        q = q->next;
    }
    // free(pipe);
    q->next = pipe->next;
    client--;
    close(pipe->fd);
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
    if (listen(fd, 32))
    {
        perror("listen");
        return 1;
    }
    struct Pipe *head = (struct Pipe *)malloc(sizeof(struct Pipe));
    struct Fd *fdt;
    // head->next=head;
    int i = 0;
    int client = 0;
    pthread_t thread[64];
    struct Pipe pipe[64];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    while (client < 32)
    {
        i = client - 1;
        int fdt;
        struct Pipe *pipe = (struct Pipe *)malloc(sizeof(struct Pipe));
        fdt = accept(fd, NULL, NULL);
        if (fdt == -1)
        {
            perror("accept");
            return 1;
        }
        if (client == 0)
        { // head
            head->fd = fdt;
            head->next = head;
            // client++;
            //  continue;
        }

        else
        {
            pipe->fd = fdt;
            pipe->next = head->next;
            head->next = pipe;
            head = pipe;
        }
        // pipe[i + 1].fd_send = head->num;
        // pipe[i + 1].fd_recv = fdt->num;
        // pipe[i].fd_send = fdt->num;
        // pipe[i].fd_recv = fdt->next->num;
        // i++;
        // pthread_exit()
        pthread_mutex_lock(&mutex);

        pthread_create(&thread[client], NULL, handle_chat, (void *)head);
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);
        // pthread_cond_signal(&cond);
        client++;
    }
    // i = 0;

    // while (i < client)
    // {
    //     // pthread_mutex_lock(&mutx);
    //     // pthread_create(&thread[i], NULL, handle_chat, (void *)&pipe[i]);
    //     // pthread_mutex_unlock(&mutx);
    //     pthread_join(thread[i], NULL);

    //     i++;
    // }
    // i = 0;
    // while (i < client)
    // {
    //     pthread_join(thread[i], NULL);
    //     i++;
    // }

    // int fd1 = accept(fd, NULL, NULL);
    // int fd2 = accept(fd, NULL, NULL);
    // if (fd1 == -1 || fd2 == -1) {
    //     perror("accept");
    //     return 1;
    // }
    // pthread_t thread1, thread2;
    // struct Pipe pipe1;
    // struct Pipe pipe2;
    // pipe1.fd_send = fd1;
    // pipe1.fd_recv = fd2;
    // pipe2.fd_send = fd2;
    // pipe2.fd_recv = fd1;
    // pthread_create(&thread1, NULL, handle_chat, (void *)&pipe1);
    // pthread_create(&thread2, NULL, handle_chat, (void *)&pipe2);
    // pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);
    return 0;
}
