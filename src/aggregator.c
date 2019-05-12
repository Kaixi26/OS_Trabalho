#ifdef _COMPILE_AGGREGATOR

#include <stdio.h>
#include "sale.h"

#define INITIAL_SIZE 1024

static struct {
    sale* sales;
    iid_t size;
} AGG;

static void agg_realloc(){
    AGG.size*=2;
    if (!realloc(AGG.sales, AGG.size * sizeof(sale)))
        return;
}

static void insert(sale s){
    if (sale_id(s) >= AGG.size)
        agg_realloc();
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
    exit(0);
}

int main (){
    start();
    for (sale s=sale_rd_stdin(); s; s=sale_rd_stdin())
        insert(s);
    end();
}

#endif
