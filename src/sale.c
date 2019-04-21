#include "sale.h"

#define SIZEOF_SALE (sizeof (struct sale))

struct sale {
    int id;
    item_price_type total_value;
    int sold_amount;
};

typedef struct sale* sale;
typedef size_t stock;

sale sale_creat (int id, item_price_type total_value, int sold_amount){
    sale s = calloc (1, sizeof (struct sale));
    if (!s)
        REP_ERR_GOTO_V2 ("Error trying to allocate sale.\n, alloc_err", alloc_err);
    s->id = id;
    s->total_value = total_value;
    s->sold_amount = sold_amount;
    return s;
alloc_err:
    return NULL;
}

void sale_dest (sale* s){
    if (*s){
        free (*s);
        *s = NULL;
    }
}

int sale_id (sale s){
    return s->id;
}

int sale_total (sale s){
    return s->total_value;
}

int sale_sold (sale s){
    return s->sold_amount;
}

int sale_write (sale s, int sale_fd){
    if (lseek (sale_fd, 0, SEEK_END) == -1)
        REP_ERR_GOTO_V2 ("Error trying to seek file.\n", seek_err);
    if (write (sale_fd, s, SIZEOF_SALE) == -1)
        REP_ERR_GOTO_V2 ("Error trying to write to file.\n", write_err);
    return 0;
write_err:
seek_err:
    return -1;
}

int stock_update (sale s, int stock_fd){
    off_t offset = sizeof(stock) * s->id;
    stock curr;
    if (lseek (stock_fd, offset, SEEK_SET) == -1)
        REP_ERR_GOTO_V2("Error trying to seek current stock\n.", seek_err);
    if (read (stock_fd, &curr, sizeof(stock)) == -1)
        REP_ERR_GOTO_V2("Error trying to read current stock\n.", read_err);
    if (lseek (stock_fd, offset, SEEK_SET) == -1)
        REP_ERR_GOTO_V2("Error trying to seek stock\n.", read_err);
    curr += s->sold_amount;
    if (write (stock_fd, &curr, sizeof(stock)) == -1)
        REP_ERR_GOTO_V2("Error trying to write current stock\n.", seek_err);
    return 0;
read_err:
seek_err:
    return -1;
}

int sale_stock_update (sale s, int stock_fd, int sale_fd){
    if (sale_write(s, sale_fd))
        goto err;
    if (stock_update (s, stock_fd) == -1)
        goto err;
    return 0;
err:
    return -1;
}

int stock_fd_write_empty (int maxid, int stock_fd){
    stock curr = 0;
    for (int i=0; i<=maxid; i++){
        if (lseek (stock_fd, i*sizeof(stock), SEEK_SET))
            REP_ERR_GOTO_V2 ("Error trying to seek stock to init.\n", seek_err);
        if (write (stock_fd, &curr, sizeof(stock)) == -1)
            REP_ERR_GOTO_V1("Error trying to write stock to init.\n", write_err);
    }
    return 0;
write_err:
seek_err:
    return -1;
}
