#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/errno.h>

#include "KnxInterface.h"

void sender(int fd)
{
    MOCK_SetMockFd(fd);

    // ADDRESS saddr = 0xAFFE;
    MOCK_setIndividualAddr(0xAFFE);

    AL_INF* al_inf = AL_getInterface();

    //  8/3.3.1 Group oriented communication
    MOCK_setHop(6);
    AL_CALL_PARAM param;
    param.hop_count_type = HOP_COUNT_NLP;
    param.asap = 1;
    param.priority = PRIORITY_LOW;
    al_inf->groupValueRead_req(&param, ACK_DONT_CARE);
}

void receiver(int fd)
{
    MOCK_SetMockFd(fd);
    MOCK_setIndividualAddr(0xAFFD);
    unsigned char data[200];
    int len = 200;
    MOCK_revPacket(data, len);
}

typedef struct {
    unsigned int  tl_ctrl:6; // octect 6
    unsigned int ap_ci:4;
    unsigned int ap_ci_data:6;
} __attribute__ ((packed)) AAA;

int main(int argc, char *argv[])
{
    // AAA a;
    // memset(&a, 0, sizeof(a));
    // // a.tl_ctrl = 63;
    // a.ap_ci = 2;
    // unsigned char *p = (unsigned char *)&a;
    // DEBUG("A...%x %x\n", *p, *(p+1));
    // return 0;

    DL_init();
    NL_init();
    TL_init();
    AL_init();
    MOCK_addGrAT(0x1001);
    MOCK_addGrOAT(1, 1);

    int socket_pair[2];
    if(socketpair(PF_LOCAL, SOCK_DGRAM, 0, socket_pair) == -1 ) {
        DEBUG("Error, socketpair create failed, errno(%d): %s\n", errno, strerror(errno));
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        DEBUG("fail fork");
    } else if (pid == 0) {
        DEBUG("send begin, %d\n", LITTLE_ENDIAN);
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

    // ADDRESS addr = 20;
    // addr = LITTLE_ENDIAN ? ((addr<<8)|(addr>>8) : addr;

    return 0;
}