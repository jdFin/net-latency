
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sched.h>

#include "options.h"
#include "util.h"
#include "priority.h"


/* how long to measure performance */
#define MEASUREMENT_TIME_SEC    5

/* number of messages to receive
 * (+1 is because we toss the first one) */
#define MESSAGE_CNT \
    ((MEASUREMENT_TIME_SEC * 1000000) / PRIORITY_MESSAGE_INTERVAL_USEC + 1)

static struct timeval msgTimevals[MESSAGE_CNT];
static uint32_t msgTimes[MESSAGE_CNT];


static void usage(void)
{
    printf("priority_recv [-p <port>] [-v] [-h]\n");
    exit(1);
}


int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_in locAddr;
    struct sockaddr_in remAddr;
    socklen_t remAddrLen;
    ssize_t nBytes;
    static char buf[PRIORITY_MESSAGE_SIZE_BYTES];
    int msgCnt;

    parse_options(argc, argv);

    if (opt_help)
        usage();

    if (opt_port == 0)
        opt_port = PRIORITY_PORT_DEFAULT;

    if (opt_prio != 0)
        {
        struct sched_param param;
        memset(&param, 0, sizeof(param));
        param.sched_priority = opt_prio;
        if (sched_setscheduler(0, SCHED_RR, &param) != 0)
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

    memset(&locAddr, 0, sizeof(locAddr));
    locAddr.sin_family = AF_INET;
    locAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    locAddr.sin_port = htons(opt_port);

    if (bind(fd, (struct sockaddr *)&locAddr, sizeof(locAddr)) != 0)
        {
        perror("bind");
        close(fd);
        exit(1);
        }

    for (msgCnt = 0; msgCnt < MESSAGE_CNT; msgCnt++)
        {

        memset(&remAddr, 0, sizeof(remAddr));
        remAddrLen = sizeof(remAddr);
        nBytes = recvfrom(fd, buf, sizeof(buf), 0,
                          (struct sockaddr *)&remAddr, &remAddrLen);
        if (nBytes < 0)
            {
            perror("recv");
            close(fd);
            exit(1);
            }

        /* message arrival time */
        if (gettimeofday(&msgTimevals[msgCnt], NULL) != 0)
            {
            perror("gettimeofday");
            close(fd);
            exit(1);
            }

        if (opt_verbose)
            printf("message received\n");

        } /* for (msgCnt...) */

    /* convert all times to useconds after the first message */
    for (msgCnt = 0; msgCnt < MESSAGE_CNT; msgCnt++)
        TIMEVAL_SUB(msgTimes[msgCnt], msgTimevals[msgCnt], msgTimevals[0]);

    /* fit line to samples 1..(MESSAGE_CNT-1)
     * http://faculty.cs.niu.edu/~hutchins/csci230/best-fit.htm
     */
    {
    double sumX = 0.0;
    double sumY = 0.0;
    double sumX2 = 0.0;
    double sumXY = 0.0;
    double xMean, yMean, slope, yInt;
    double jLo, jHi, jSum;

    for (msgCnt = 1; msgCnt < MESSAGE_CNT; msgCnt++)
        {
        sumX += msgCnt;
        sumY += msgTimes[msgCnt];
        sumX2 += (msgCnt * msgCnt);
        sumXY += ((double)msgCnt * (double)msgTimes[msgCnt]);
        }
    xMean = sumX / (MESSAGE_CNT - 1);
    yMean = sumY / (MESSAGE_CNT - 1);
    slope = (sumXY - sumX * yMean) / (sumX2 - sumX * xMean);
    yInt = yMean - slope * xMean;

    /* find max jitter */
    jLo = 0.0;
    jHi = 0.0;
    jSum = 0.0;
    for (msgCnt = 1; msgCnt < MESSAGE_CNT; msgCnt++)
        {
        double j = msgTimes[msgCnt] - (slope * msgCnt + yInt);
        if (jLo > j)
            jLo = j;
        if (jHi < j)
            jHi = j;
        if (j < 0)
            j = -j;
        jSum += j;
        }

    printf("jitter: %0.1f...%0.1f usec\n", jLo, jHi);
    printf("        %0.1f avg\n", jSum / (MESSAGE_CNT - 1));
    
    }

    exit(0);

} /* main */
