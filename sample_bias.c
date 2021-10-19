#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int r1(uint16_t max)
{
    uint64_t rmax;
    uint16_t rem;
    int res;

    rmax = (uint64_t)RAND_MAX + 1;
    rem = rmax % max;
    do
    {
        /* Biased if we don't a few samples! */
        res = rand() ^ rand() ^ rand();
    } while (((uint64_t)res) > (rmax - rem));
    return res % max;
}

static int r0(uint16_t max)
{
    uint64_t rmax;
    uint16_t rem;
    int res;

    rmax = (uint64_t)RAND_MAX + 1;
    rem = rmax % max;
    do
    {
        /* Biased if we don't a few samples! */
        res = rand();
    } while (((uint64_t)res) > (rmax - rem));
    return res % max;
}

#define RUNS 10000000
#define DAYS_IN_YEAR 365

unsigned hist[2][DAYS_IN_YEAR];

int main(void)
{
    unsigned count;

    srand(time(NULL));
    count = 0;
    while (count++ < RUNS)
    {
        int n0, n1;
        n0 = r0(DAYS_IN_YEAR);
        n1 = r1(DAYS_IN_YEAR);
        hist[0][n0]++;
        hist[1][n1]++;
    }

    {
        unsigned i;
        for (i = 0; i < DAYS_IN_YEAR; i++)
        {
            printf("%3u, %8u, %8u\n", i, hist[0][i], hist[1][i]);
        }
    }

    return 0;
}
