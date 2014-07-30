#ifndef util_h
#define util_h


/*
 * D = T1 - T2
 * T1 and T2 are struct timeval
 * D is anything that an integer can automatically cast to
 */
#define TIMEVAL_SUB(D, T1, T2) \
    do \
        { \
        if ((T1).tv_usec < (T2).tv_usec) \
            { \
            /* borrow */ \
            (D) = (T1).tv_usec + 1000000 - (T2).tv_usec; \
            (D) += (((T1).tv_sec - 1 - (T2).tv_sec) * 1000000); \
            } \
        else \
            { \
            /* no borrow */ \
            (D) = (T1).tv_usec - (T2).tv_usec; \
            (D) += (((T1).tv_sec - (T2).tv_sec) * 1000000); \
            } \
        } \
    while (0)


/*
 * T += U, where U <= 1000000
 * T is struct timeval
 * U is anything that can sum with an integer
 */
#define TIMEVAL_INC(T, U) \
    do \
        { \
        (T).tv_usec += (U); \
        if ((T).tv_usec >= 1000000) \
            { \
            /* carry */ \
            (T).tv_usec -= 1000000; \
            (T).tv_sec++; \
            } \
        } \
    while (0)


/*
 * D = T1 - T2
 * T1 and T2 are struct timespec
 * D is anything that an integer can automatically cast to
 */
#define TIMESPEC_SUB(D, T1, T2) \
    do \
        { \
        if ((T1).tv_nsec < (T2).tv_nsec) \
            { \
            /* borrow */ \
            (D) = (T1).tv_nsec + 1000000000 - (T2).tv_nsec; \
            (D) += (((T1).tv_sec - 1 - (T2).tv_sec) * 1000000000); \
            } \
        else \
            { \
            /* no borrow */ \
            (D) = (T1).tv_nsec - (T2).tv_nsec; \
            (D) += (((T1).tv_sec - (T2).tv_sec) * 1000000000); \
            } \
        } \
    while (0)


/*
 * T += U, where U <= 1000000000
 * T is struct timespec
 * U is anything that can sum with an integer
 */
#define TIMESPEC_INC(T, U) \
    do \
        { \
        (T).tv_nsec += (U); \
        if ((T).tv_nsec >= 1000000000) \
            { \
            /* carry */ \
            (T).tv_nsec -= 1000000000; \
            (T).tv_sec++; \
            } \
        } \
    while (0)


/*
 * Compare T1 and T2
 * T1 > T2; return 1
 * T1 < T2; return -1
 * T1 == T2; return 0
 */
#define TIMESPEC_CMP(T1, T2) \
    ( \
    ((T1).tv_sec > (T2).tv_sec) ? 1 : \
        ( \
        ((T1).tv_sec < (T2).tv_sec) ? -1 : \
            ( \
            ((T1).tv_nsec > (T2).tv_nsec) ? 1 : \
                ( \
                ((T1).tv_nsec < (T2).tv_nsec) ? -1 : 0 \
                ) \
            ) \
        ) \
    )


#endif /* util_h */
