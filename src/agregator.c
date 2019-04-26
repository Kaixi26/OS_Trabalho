#ifdef _COMPILE_AGGREGATOR

#include "debug.h"
#include "sale.h"
#include <stdio.h>

#define INITIAL_SIZE 1024

typedef struct aggregate {
    sale* sales;
    id_type size;
}* aggregate;

static id_type agg_index (sale s){
    return sale_id (s) - 1;
}

static aggregate agg_creat (void){
    aggregate tmp;
    if ((tmp = calloc (1, sizeof (struct aggregate))) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to allocate aggregate.\n", agg_alloc_err);
    tmp->size = INITIAL_SIZE;
    if ((tmp->sales = calloc (tmp->size, sizeof (sale*))) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to allocate aggregate.\n", sales_alloc_err);
    return tmp;
sales_alloc_err:
    free (tmp);
agg_alloc_err:
    return NULL;
}

static void agg_realloc_to_fit (aggregate agg, id_type i){
    if (agg->size >= i)
        return;
    id_type new_size = agg->size;
    sale* new_sales;
    while (new_size < i)
        new_size*=2;
    new_sales = calloc (new_size, sizeof (sale));
    memcpy (new_sales, agg->sales, agg->size * sizeof(sale));
    agg->size = new_size;
    agg->sales = new_sales;
}

static void agg_free (aggregate agg){
    if (agg){
        for (int i=0; i<agg->size; i++)
            sale_free (agg->sales[i]);
        free (agg->sales);
        free (agg);
    }
}

static void sale_err (sale s){
    fprintf (stderr, "%d %d %ld\n", sale_id (s), sale_sold (s), sale_total (s));
}

static void agg_add (aggregate agg, sale s){
    if (agg->sales[agg_index (s)]){
        sale_add (agg->sales[agg_index (s)], s);
        sale_free (s);
    }
    else agg->sales[agg_index (s)] = s;
}

static void agg_output (aggregate agg){
    for (int i=0; i<agg->size; i++)
        if (agg->sales[i])
            sale_write (STDOUT_FILENO, agg->sales[i]);
}

int main (){
    sale s;
    aggregate agg = agg_creat ();
    while ((s = sale_read_next (STDIN_FILENO)) != NULL){
        agg_add (agg, s);
    }
    agg_output (agg);
    agg_free (agg);
    return 0;
}
#endif
