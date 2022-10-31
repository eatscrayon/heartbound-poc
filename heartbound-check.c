#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <memory.h>

in_addr_t inet_addr(const char *cp);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

typedef struct PASSING_PARAMS
{
    unsigned int *badcert;
    unsigned int len;
    unsigned int in;
} PassingParams;

void swap(unsigned int *a, unsigned int *b)
{
    unsigned int tmp = *a;
    *a = *b;
    *b = tmp;
}

void *poc(void *ptr)
{

    PassingParams *paramPtr = (PassingParams *)ptr;
    unsigned int S[256];
    unsigned int j = 0;
    unsigned int i = 0;
    unsigned int k = 0;
    unsigned int rnd = 0;
    for (i = 0; i < 256; i++)
    {
        S[i] = i;
    }
    i = 0;
    j = 0;
    int n = 0;
    printf("Reading 172 bytes of memory: \n");
    while (n < paramPtr->len)
    {
        unsigned char bufA[1];

        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        swap(&S[i], &S[j]);
        rnd = S[(S[i] + S[j]) % 256];
        bufA[0] = rnd ^ paramPtr->badcert[n++ % paramPtr->len];
        printf("%c", bufA[0]);
    }
    printf("Done!\n");
}

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("Usage: ./heartbound [IP] [PORT]");
        exit(1);
    }

    char REMOTE_ADDR[30];
    strcpy(REMOTE_ADDR, argv[1]);
    int TCP_PORT = atoi(argv[2]);

    struct sockaddr_in server;

    signal(SIGCHLD, SIG_IGN);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(REMOTE_ADDR);
    server.sin_port = htons(TCP_PORT);

    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        exit(1);
    }

    if (fork() == 0)
    {

        if ((connect(sock, (struct sockaddr *)&server, sizeof server)) != 0)
        {
            close(sock);
        }

        int badcert[256] =
            {76, 96, 113, 104, 127, 55, 120, 71, 70, 86, 83, 104, 49, 12, 3, 227,
             162, 217, 219, 188, 255, 131, 39, 1, 56, 46, 17, 225, 208, 225, 134, 234,
             82, 5, 68, 24, 18, 192, 185, 214, 57, 165, 51, 132, 182, 214, 85, 90,
             5, 104, 214, 254, 139, 43, 150, 241, 191, 94, 25, 79, 178, 72, 9, 70,
             180, 182, 104, 195, 82, 131, 221, 95, 132, 185, 118, 33, 69, 95, 97, 252,
             182, 109, 229, 215, 127, 120, 46, 39, 22, 99, 70, 216, 113, 15, 185, 17,
             231, 60, 72, 63, 7, 3, 99, 17, 239, 191, 144, 229, 49, 172, 9, 124,
             24, 77, 47, 132, 63, 253, 7, 247, 239, 243, 122, 156, 142, 197, 26, 18,
             229, 187, 2, 147, 47, 205, 93, 177, 85, 140, 227, 252, 230, 145, 244, 236,
             79, 156, 65, 117, 20, 241, 138, 198, 11, 238, 11, 149, 236, 221, 171, 145,
             207, 227, 216, 4, 60, 214, 244, 98, 212, 187, 118, 118};

        pthread_t reader;
        PassingParams *buffer_to_socket;
        buffer_to_socket = (PassingParams *)malloc(sizeof(PassingParams));
        buffer_to_socket->badcert = badcert;
        buffer_to_socket->len = 172;
        buffer_to_socket->in = sock;
        pthread_create(&reader, NULL, &poc, (void *)buffer_to_socket);

        // clean up threads and exit if sock is disconnected
        struct pollfd pfd;
        pfd.fd = sock;
        pfd.events = POLLIN | POLLHUP | POLLRDNORM;
        pfd.revents = 0;
        while (pfd.revents == 0)
        {
            if (poll(&pfd, 1, 100) > 0)
            {
                char buffer[32];
                if (recv(sock, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0)
                {
                    kill(reader, SIGKILL);
                    close(sock);
                    exit(0);
                }
            }
            while (1)
            {
                sleep(10);
            }
        }
    }

    while (1)
    {
        sleep(10);
    }

    return 0;
}