#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/errno.h>

#include "KnxInterface.h"

void sender(int fd)
{
    DL_SetMockFd(fd);
}

void receiver(int fd)
{
    DL_SetMockFd(fd);
}

int main(int argc, char *argv[])
{
    // if (argc < 2) {
    //     DEBUG("fail: parameter\n");
    //     return -1;
    // }

    // DEBUG("param: %s\n", argv[1]);

    DL_init();
    NL_init();
    TL_init();
    AL_init();

    int socket_pair[2];
    if(socketpair(PF_LOCAL, SOCK_DGRAM, 0, socket_pair) == -1 ) {
        DEBUG("Error, socketpair create failed, errno(%d): %s\n", errno, strerror(errno));
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        DEBUG("fail fork");
    } else if (pid == 0) {
        DEBUG("send begin\n");
        char str[] = "hello world";
        write(socket_pair[0], "hello world", strlen(str));
        sender(socket_pair[0]);
    } else {
        char str[100];
        memset(str, 0, sizeof(str));
        int n = read(socket_pair[1], str, 100);
        DEBUG("reeive begin, %d, %s\n", n, str);
        receiver(socket_pair[1]);
    }



    return 0;
}