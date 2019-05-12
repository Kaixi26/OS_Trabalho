#ifdef _COMPILE_PPRINTER

#include <stdio.h>
#include "item.h"
#include "files.h"
#include "stock.h"
#include "sale.h"

static struct {
    int fd_item;
    int fd_item_n;
    int fd_stock;
    int fd_sales;
} PRNT;

void start (){
    PRNT.fd_item   = open(ITEM_FP , O_RDONLY);
    PRNT.fd_item_n = open(INAME_FP, O_RDONLY);
    PRNT.fd_stock  = open(STOCK_FP, O_RDWR | O_CREAT, 0666);
    PRNT.fd_sales  = open(SALES_FP, O_RDWR | O_CREAT, 0666);
}

static void pprint_items (){
    iid_t max = item_id_top(PRNT.fd_item);
    printf("N. of items : %ld\n", max-1);
    item it;
    for (iid_t i = 1; i < max; i++){
        it = item_rd(i, PRNT.fd_item, PRNT.fd_item_n);
        printf("%ld\t%.2f\t%s\n", i, item_price(it), item_name(it));
        free(it);
    }
}

static void pprint_stock (){
    iid_t max = item_id_top(PRNT.fd_item);
    for (iid_t i = 1; i<max; i++)
        printf ("%ld\t%ld\n", i, stock_rd(i, PRNT.fd_stock));
}

static void pprint_sales(){
    uint64_t i=0;
    sale s;
    while((s = sale_rd(i++, PRNT.fd_sales)) != NULL){
        printf ("sale\t%ld\t%.2f\t%ld\n", sale_id(s), sale_value(s), sale_amount(s));
        sale_free(s);
    }
}

static void pprint_aggregate(){
    for (sale s=sale_rd_stdin(); s; s=sale_rd_stdin()){
        printf ("sale\t%ld\t%.2f\t%ld\n", sale_id(s), sale_value(s), sale_amount(s));
        sale_free(s);
    }
}

int main (int argc, char** argv){
    if (argc != 2) exit(0);
    start();
    switch (atol(argv[1])){
    case 1:
        pprint_items();
        break;
    case 2:
        pprint_stock();
        break;
    case 3:
        pprint_sales();
        break;
    case 4:
        pprint_aggregate();
        break;
    }
    return 0;
}

#endif
