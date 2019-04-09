#include <time.h>

void __sleep_us__(int sleep_us)
{
    struct timespec ts = {
        .tv_sec = sleep_us / 1000000,
        .tv_nsec = (sleep_us % 1000000) * 1000
    };

    while (clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, &ts));
}
