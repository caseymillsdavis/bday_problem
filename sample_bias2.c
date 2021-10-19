#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int r(uint16_t max)
{
    uint64_t rmax;
    uint16_t rem;
    int res;

    rmax = (uint64_t)RAND_MAX + 1;
    rem = rmax % max;
    do
    {
        res = rand();
    } while (((uint64_t)res) > (rmax - rem));
    return res % max;
}

#define RUNS 10000000
#define DAYS_IN_YEAR 365

#define BINS 110
unsigned hist[BINS];

/* Need a set. Implement as fixed size bit map. */

uint8_t bmap[(DAYS_IN_YEAR + 7) / 8];

/* Returns 1 if already contained. */
int set_insert(uint16_t e)
{
    int ret;
    unsigned byte;
    unsigned bit;

    byte = e/8;
    bit = e%8;

    if (bmap[byte] & (1 << bit))
    {
        ret = 1;
    }
    else
    {
        ret = 0;
        bmap[byte] |= (1 << bit);
    }

    return ret;
}

unsigned rand_hist[DAYS_IN_YEAR];

int main(void)
{
    unsigned count;

    srand(time(NULL));

    count = 0;
    while (count++ < RUNS)
    {
        unsigned n;
        unsigned v;

        memset(bmap, 0, sizeof(bmap));
        n = 0;

        do
        {
            n++;
            v = r(DAYS_IN_YEAR);
            rand_hist[v]++;
        } while (!set_insert(v));

        if (n >= BINS)
        {
            hist[BINS-1]++;
        }
        else
        {
            hist[n]++;
        }
    }

#if 0
    {
        unsigned n;
        unsigned total;
        unsigned i;

        total = 0;
        n = 0;
        for (i = 0; i < BINS; i++)
        {
            if (total < RUNS/2)
            {
                n++;
                total += hist[i];
            }
            printf("%3d, %8u\n", i, hist[i]);
        }
        printf("~50% (%u) at %u\n", total, n-1);
    }
#endif

    {
        unsigned i;
        for (i = 0; i < DAYS_IN_YEAR; i++)
        {
            printf("%3u, %8u\n", i, rand_hist[i]);
        }
    }

    return 0;
}
