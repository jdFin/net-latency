
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "util.h"


#define show_getres(clock) \
    do \
        { \
        struct timespec ts; \
        clock_getres((clock), &ts); \
        printf("  %24s %ld\n", #clock, ts.tv_nsec); \
        } \
    while (0)


static void usage(char const * progName)
    {
    printf("Usage: %s -d <delay_us> -t <time_s> -h\n", progName);
    exit(1);
    }


int main(int argc, char *argv[])
    {
    char c;
    int delay_us = 1000;
    int time_s = 1;
    struct timespec tsStart;
    struct timespec tsWake;
    struct timespec tsWoke;
    struct timespec tsEnd;
    unsigned delta;
    unsigned deltaMin = (unsigned)(-1);
    unsigned deltaMax = 0;
    unsigned deltaSum = 0;
    unsigned deltaCnt = 0;

    while ((c = getopt(argc, argv, "d:ht:")) != -1)
        {
        switch (c)
            {
            case 'd':
                delay_us = atoi(optarg);
                break;
            case 't':
                time_s = atoi(optarg);
                break;
            default:
                usage(argv[0]);
                /*NOTREACHED*/
            }
        }

    printf("delay_us=%d, time_s=%d\n", delay_us, time_s);

    printf("clock_getres:\n");
    show_getres(CLOCK_REALTIME);
    show_getres(CLOCK_REALTIME_COARSE);
    show_getres(CLOCK_MONOTONIC);
    show_getres(CLOCK_MONOTONIC_COARSE);
    show_getres(CLOCK_MONOTONIC_RAW);
    show_getres(CLOCK_BOOTTIME);
    show_getres(CLOCK_PROCESS_CPUTIME_ID);
    show_getres(CLOCK_THREAD_CPUTIME_ID);

    clock_gettime(CLOCK_REALTIME, &tsStart);
    tsEnd.tv_nsec = tsStart.tv_nsec;
    tsEnd.tv_sec = tsStart.tv_sec + time_s;
    tsWake = tsStart;
    do
        {
        TIMESPEC_INC(tsWake, delay_us * 1000);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &tsWake, NULL);
        clock_gettime(CLOCK_REALTIME, &tsWoke);
        TIMESPEC_SUB(delta, tsWoke, tsWake);
        if (deltaMin > delta)
            deltaMin = delta;
        if (deltaMax < delta)
            deltaMax = delta;
        deltaSum += delta;
        deltaCnt++;
        }
    while (TIMESPEC_CMP(tsWake, tsEnd) < 0);

    printf ("deltaMin=%u deltaMax=%u deltaAvg=%u\n", deltaMin, deltaMax, deltaSum / deltaCnt);

    return 0;
    }
