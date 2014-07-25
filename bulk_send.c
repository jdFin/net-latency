
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "options.h"
#include "bulk.h"


static void usage(void)
{
    printf("bulk_send -i <ipaddr> [-p <port>] [-h]\n");
    exit(1);
}


int main(int argc, char *argv[])
{
    struct in_addr dstIp;
    int fd;
    struct sockaddr_in remAddr;
    static char buf[BULK_MESSAGE_SIZE_BYTES];

    parse_options(argc, argv);

    if (opt_help)
        usage();

    if (opt_ip == NULL)
        usage();

    if (inet_pton(AF_INET, opt_ip, &dstIp) != 1)
        usage();

    if (opt_port == 0)
        opt_port = BULK_PORT_DEFAULT;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        {
        perror("socket");
        exit(1);
        }

    memset(&remAddr, 0, sizeof(remAddr));
    remAddr.sin_family = AF_INET;
    remAddr.sin_addr.s_addr = dstIp.s_addr;
    remAddr.sin_port = htons(opt_port);

    while (1)
        {

        if (sendto(fd, &buf, sizeof(buf), 0, (struct sockaddr *)&remAddr,
                   sizeof(remAddr)) != sizeof(buf))
            {
            perror("sendto");
            close(fd);
            exit(1);
            }

        } /* while (1) */

} /* main */
