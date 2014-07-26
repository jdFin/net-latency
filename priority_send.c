
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sched.h>

#include "options.h"
#include "util.h"
#include "priority.h"

/* forwards */
static void setIpTos(int fd, uint8_t tos);
static void setSockPrio(int fd);


static void usage(void)
{
    printf("priority_send -i <ipaddr> [-p <port>] [-m <method>] [-h]\n");
    exit(1);
}


int main(int argc, char *argv[])
{
    struct in_addr dstIp;
    int fd;
    struct sockaddr_in remAddr;
    static char buf[PRIORITY_MESSAGE_SIZE_BYTES];
    struct timeval now;
    struct timeval msgTime;
    useconds_t delay_us;

    parse_options(argc, argv);

    if (opt_help)
        usage();

    if (opt_port == 0)
        opt_port = PRIORITY_PORT_DEFAULT;

    if (opt_ip == NULL)
        usage();

    if (inet_pton(AF_INET, opt_ip, &dstIp) != 1)
        usage();

    if (opt_prio != 0)
        {
        struct sched_param param;
        memset(&param, 0, sizeof(param));
        param.sched_priority = opt_prio;
        /* SCHED_RR introduces lots of jitter */
        if (sched_setscheduler(0, SCHED_FIFO, &param) != 0)
            {
            perror("sched_setscheduler");
            exit(1);
            }
        }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        {
        perror("socket");
        exit(1);
        }

    if (opt_tos != -1)
        setIpTos(fd, opt_tos);

    if (opt_sock)
        setSockPrio(fd);

    memset(&remAddr, 0, sizeof(remAddr));
    remAddr.sin_family = AF_INET;
    remAddr.sin_addr.s_addr = dstIp.s_addr;
    remAddr.sin_port = htons(opt_port);

    if (gettimeofday(&now, NULL) != 0)
        {
        perror("gettimeofday");
        close(fd);
        exit(1);
        }

    /* msgTime is at least one interval in the future */
    msgTime.tv_sec = now.tv_sec + 1;
    msgTime.tv_usec = PRIORITY_MESSAGE_INTERVAL_USEC;

    while (1)
        {

        /* sleep until the next message time */

        if (gettimeofday(&now, NULL) != 0)
            {
            perror("gettimeofday");
            close(fd);
            exit(1);
            }
        TIMEVAL_SUB(delay_us, msgTime, now);
        if (usleep(delay_us) != 0)
            {
            perror("usleep");
            close(fd);
            exit(1);
            }

        /* send message */

        if (sendto(fd, &buf, sizeof(buf), 0, (struct sockaddr *)&remAddr,
                   sizeof(remAddr)) != sizeof(buf))
            {
            perror("sendto");
            close(fd);
            exit(1);
            }

        TIMEVAL_INC(msgTime, PRIORITY_MESSAGE_INTERVAL_USEC);

        } /* while (1) */

} /* main */


/*
 * Set IPPROTO_IP/IP_TOS
 * This sets the IP header TOS field.
 * netinet/ip.h says this is deprecated; do it with DSCP/CS instead.
 *
 * With "bulk" traffic saturating the outbound link, and "priority" traffic
 * using this setting:
 * tos=0x10 is not sufficient to push priority traffic through
 * tos=0xd0 is sufficient
 */
static void setIpTos(int fd, uint8_t tos)
{
    uint8_t tos0, tos1;
    socklen_t tosLen;

    tosLen = sizeof(tos0);
    if (getsockopt(fd, IPPROTO_IP, IP_TOS, &tos0, &tosLen) != 0)
        {
        perror("getsockopt");
        close(fd);
        exit(1);
        }
    if (tosLen != 1)
        {
        printf("tosLen != 1\n");
        close(fd);
        exit(1);
        }

    if (setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) != 0)
        {
        perror("setsockopt");
        close(fd);
        exit(1);
        }

    tosLen = sizeof(tos1);
    if (getsockopt(fd, IPPROTO_IP, IP_TOS, &tos1, &tosLen) != 0)
        {
        perror("getsockopt");
        close(fd);
        exit(1);
        }

    printf("tos0 = 0x%02x; set to 0x%02x; tos1 = 0x%02x\n", tos0, tos, tos1);

} /* setIpTos */


/*
 * Set SOL_SOCKET/SO_PRIORITY
 * This does not appear to set the IP header's TOS field.
 */
static void setSockPrio(int fd)
{
    int soPrio;
    socklen_t soPrioLen;

    soPrio = -99;
    soPrioLen = sizeof(soPrio);
    if (getsockopt(fd, SOL_SOCKET, SO_PRIORITY, &soPrio, &soPrioLen) != 0)
        {
        perror("getsockopt");
        close(fd);
        exit(1);
        }
    printf("soPrio is %d; soPrioLen is %d\n", soPrio, soPrioLen);

    soPrio = 6;
    if (setsockopt(fd, SOL_SOCKET, SO_PRIORITY, &soPrio, sizeof(soPrioLen)) != 0)
        {
        perror("setsockopt");
        close(fd);
        exit(1);
        }
    printf("soPrio set to %d\n", 6);

    soPrio = -99;
    soPrioLen = sizeof(soPrio);
    if (getsockopt(fd, SOL_SOCKET, SO_PRIORITY, &soPrio, &soPrioLen) != 0)
        {
        perror("getsockopt");
        close(fd);
        exit(1);
        }
    printf("soPrio is %d; soPrioLen is %d\n", soPrio, soPrioLen);

} /* setSockPrio */
