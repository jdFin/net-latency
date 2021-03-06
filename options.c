
#include <unistd.h>
#include <stdlib.h>
#include "options.h"


/* -p <port>            UDP port to send to or receive on
 *                      bulk_send
 *                      bulk_recv
 *                      priority_send
 *                      priority_recv
 */
short opt_port = 0;

/* -i <ip_addr>         IP address to send to
 *                      bulk_send
 *                      priority_send
 */
char const * opt_ip = NULL;

/* -s                   whether to set socket options
 *                      priority_send
 */
int opt_sock = 0;

/* -t <tos>             TOS to set
 *                      priority_send
 */
int opt_tos = -1;

/* -v                   verbose status output
 *                      priority_recv
 */
int opt_verbose = 0;

/* -h                   print help and exit
 *                      bulk_send
 *                      bulk_recv
 *                      priority_send
 *                      priority_recv
 */
int opt_help = 0;

/* -r <prio>            real-time priority
 *                      priority_send
 *                      priority_recv
 */
int opt_prio = 0;



void parse_options(int argc, char * const argv[])
{
    char c;
    char const * opts = "i:r:sp:t:vh";

    while ((c = getopt(argc, argv, opts)) != -1)
        {
        switch (c)
            {
            case 'i':
                opt_ip = optarg;
                break;
            case 's':
                opt_sock = 1;
                break;
            case 'r':
                opt_prio = atoi(optarg);
                break;
            case 'p':
                opt_port = atoi(optarg);
                break;
            case 't':
                opt_tos = strtol(optarg, NULL, 0);
                break;
            case 'v':
                opt_verbose = 1;
                break;
            case 'h':
                opt_help = 1;
                break;
            }
        }

} /* parse_options */
