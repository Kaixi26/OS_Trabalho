#ifdef _COMPILE_AGGREGATOR

#include <stdio.h>
#include "sale.h"

#define INITIAL_SIZE 8

static struct {
    sale* sales;
    iid_t size;
} AGG;

static void agg_realloc(iid_t id){
    while (AGG.size <= id)
        AGG.size*=2;
    void *tmp = realloc (AGG.sales, AGG.size * sizeof(sale*));
    if (!tmp) return;
    AGG.sales = tmp;
}

static void insert(sale s){
    agg_realloc(sale_id(s));
    if (AGG.sales[sale_id(s)] == NULL)
        AGG.sales[sale_id(s)] = s;
    else {
        sale_merge(AGG.sales[sale_id(s)], s);
        sale_free (s);
    }
}

static void start(){
    AGG.size = INITIAL_SIZE;
    AGG.sales = calloc(AGG.size, sizeof(sale));
}

static void end(){
    for (int i=0; i < AGG.size; i++)
        if (AGG.sales[i])
            sale_wr_stdout(AGG.sales[i]);
    fprintf (stderr, "hi\n");
    exit(0);
}

int main (){
    start();
    for (sale s=sale_rd_stdin(); s; s=sale_rd_stdin())
        insert(s);
    end();
}

#endif
