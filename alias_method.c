#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <sys/queue.h>

SLIST_HEAD(int_list, int_item); 
struct int_item
{
    int i;
    SLIST_ENTRY(int_item) item_entry;
};

typedef struct alias_method_s
{
    unsigned len;
    unsigned *alias;
    double *prob;
} alias_method_t;

void alias_method_init(alias_method_t *am, double *p, unsigned len)
{
    struct int_list small = SLIST_HEAD_INITIALIZER(small);
    struct int_list large = SLIST_HEAD_INITIALIZER(large);
    double *pc;
    double *prob;
    unsigned *alias;

    assert(len != 0);

    am->len = len;
    am->alias = malloc(sizeof(unsigned)*len);
    am->prob = malloc(sizeof(double)*len);
    assert(am->alias && am->prob);

    pc = malloc(sizeof(double)*len);
    assert(pc);

    prob = am->prob;
    alias = am->alias;

    SLIST_INIT(&small);
    SLIST_INIT(&large);

    {
        unsigned i;
        for (i = 0; i < len ; i++)
        {
            struct int_item *item;

            item = malloc(sizeof(*item));
            assert(item);

            item->i = i;
            pc[i] = p[i]*len;
            if (pc[i] < 1)
            {
                SLIST_INSERT_HEAD(&small, item, item_entry);
            }
            else
            {
                SLIST_INSERT_HEAD(&large, item, item_entry);
            }
        }
    }

#if 0
    {
        struct int_item *item;
        printf("small list:\n");
        SLIST_FOREACH(item, &small, item_entry)
        {
            printf("%d\n", item->i);
        }
        printf("large list:\n");
        SLIST_FOREACH(item, &large, item_entry)
        {
            printf("%d\n", item->i);
        }
    }
#endif

    while (!SLIST_EMPTY(&small) && !SLIST_EMPTY(&large))
    {
        struct int_item *l;
        struct int_item *g;

        l = SLIST_FIRST(&small);
        g = SLIST_FIRST(&large);

        SLIST_REMOVE_HEAD(&small, item_entry);
        SLIST_REMOVE_HEAD(&large, item_entry);

        prob[l->i] = pc[l->i];
        alias[l->i] = g->i; 
        pc[g->i] = pc[g->i] + pc[l->i] - 1;

        if (pc[g->i] < 1)
        {
            SLIST_INSERT_HEAD(&small, g, item_entry);
        }
        else
        {
            SLIST_INSERT_HEAD(&large, g, item_entry);
        }

        free(l);
    }

    while (!SLIST_EMPTY(&large))
    {
        struct int_item *g;
        g = SLIST_FIRST(&large);
        SLIST_REMOVE_HEAD(&large, item_entry);
        prob[g->i] = 1.0f;
        free(g);
    }

    while (!SLIST_EMPTY(&small))
    {
        struct int_item *l;
        l = SLIST_FIRST(&small);
        SLIST_REMOVE_HEAD(&small, item_entry);
        prob[l->i] = 1.0f;
        free(l);
    }

#if 0
    {
        unsigned i;
        for (i = 0; i < len; i++)
        {
            printf("%4u %.4f\n", alias[i], prob[i]);
        }
    }
#endif

    free(pc);
}

#if 0
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
#endif

static int biased_coin(double p)
{
    if (((double)rand()/RAND_MAX) > p)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int alias_method_generate(alias_method_t *am)
{
    int roll;
    int flip;

    roll = r(am->len);
    flip = biased_coin(am->prob[roll]);
    if (flip)
    {
        return roll;
    }
    else
    {
        return am->alias[roll];
    }
}

void histogram(unsigned *a, unsigned len, unsigned n)
{
    unsigned h[n];
    unsigned i;

    memset(h, 0, sizeof(h));
    for (i = 0; i < len; i++)
    {
        h[a[i]]++;
    }

    for (i = 0; i < n; i++)
    {
        printf("%u: %.6f\n", i, (double)h[i]/len);
    }
}

#define ITEM_N 366
#define SAMPLES 1000000
unsigned vs[SAMPLES];

#include <bday_data.h>


#define DAYS_IN_YEAR 366
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
double bday_plist[DAYS_IN_YEAR];
void process_bday_data(void)
{
    unsigned i;
    unsigned total;

    assert(DAYS_IN_YEAR == ARRAY_SIZE(bday_totals));

    total = 0;
    for (i = 0; i < DAYS_IN_YEAR; i++)
    {
        total += bday_totals[i].total; 
    }

    for (i = 0; i < DAYS_IN_YEAR; i++)
    {
        bday_plist[i] = ((float)bday_totals[i].total)/total;
    }
}

#if 0
int main(void)
{
    alias_method_t am;
    //double plist[] = {.05f, .10f, .20f, .40f, .25f};
    //double plist[] = {.9f, .025f, .025f, .025f, .025f};

    process_bday_data();

    alias_method_init(&am, bday_plist, ITEM_N);
    
    srand(time(NULL));

    {
        unsigned i;
        for (i = 0; i < SAMPLES; i++)
        {
            int v;
            v = alias_method_generate(&am);
            vs[i] = v;
        }
    }

    histogram(vs, SAMPLES, ITEM_N);

    return 0;
}
#endif
