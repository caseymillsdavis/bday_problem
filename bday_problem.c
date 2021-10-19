#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int r(unsigned max)
{
    uint64_t rmax;
    uint16_t rem;
    int res;

    rmax = (uint64_t)RAND_MAX + 1;
    rem = rmax % max;
    do
    {
        res = rand() ^ rand() ^ rand();
    } while (((uint64_t)res) > (rmax - rem));
    return res % max;
}

#define RUNS 1000000
#define DAYS_IN_YEAR 366

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

#include <alias_method.c>

int main(void)
{
    alias_method_t am;
    unsigned count;

    srand(time(NULL));

    process_bday_data();
    alias_method_init(&am, bday_plist, ITEM_N);

    count = 0;
    while (count++ < RUNS)
    {
        unsigned n;

        memset(bmap, 0, sizeof(bmap));
        n = 0;

        do
        {
            n++;
        //} while (!set_insert(r(DAYS_IN_YEAR)));
        } while (!set_insert(alias_method_generate(&am)));

        if (n >= BINS)
        {
            hist[BINS-1]++;
        }
        else
        {
            hist[n]++;
        }
    }

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

    return 0;
}
