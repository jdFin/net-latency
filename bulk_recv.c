
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>

#include "options.h"
#include "bulk.h"


/* how often to print data rate */
#define PRINT_INTERVAL_SEC 2


int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_in locAddr;
    struct sockaddr_in remAddr;
    socklen_t remAddrLen;
    ssize_t nBytes;
    static char buf[BULK_MESSAGE_SIZE_BYTES];
    struct timeval now;
    struct timeval printTime;
    struct timeval timeout;
    fd_set fds;
    int retVal;
    int totalBytes;

    parse_options(argc, argv);

    if (opt_port == 0)
        opt_port = BULK_PORT_DEFAULT;

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

    if (gettimeofday(&now, NULL) != 0)
        {
        perror("gettimeofday");
        close(fd);
        exit(1);
        }

    /* first print time is rounded down */
    printTime.tv_sec = now.tv_sec + PRINT_INTERVAL_SEC;
    printTime.tv_usec = 0;

    totalBytes = 0;

    while (1)
        {

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;
        retVal = select(fd + 1, &fds, NULL, NULL, &timeout);

        if (retVal < 0)
            {
            perror("select");
            close(fd);
            exit(1);
            }

        if (retVal > 0)
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
            totalBytes += nBytes;
            }

        if (gettimeofday(&now, NULL) != 0)
            {
            perror("gettimeofday");
            close(fd);
            exit(1);
            }

        if (now.tv_sec >= printTime.tv_sec)
            {
            printf("%d bytes/sec\n", totalBytes / PRINT_INTERVAL_SEC);
            printTime.tv_sec += PRINT_INTERVAL_SEC;
            totalBytes = 0;
            }

        } /* while (1) */

} /* main */
