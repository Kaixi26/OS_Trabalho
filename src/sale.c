#include "sale.h"

#define SIZEOF_SALE (sizeof (struct sale))

struct sale {
    id_type id;
    price_type total_value;
    item_amnt_type sold_amount;
};

typedef struct sale* sale;

sale sale_creat (int id, price_type total_value, int sold_amount){
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

void sale_free (sale s){
    if (s){
        free (s);
        s = NULL;
    }
}

int sale_id (sale s){
    return s->id;
}

price_type sale_total (sale s){
    return s->total_value;
}

int sale_sold (sale s){
    return s->sold_amount;
}

sale sale_read (size_t i, int sale_fd){
    off_t offset = SIZEOF_SALE * i;
    off_t end;
    sale s;
    if ((s = calloc (1, SIZEOF_SALE)) == NULL)
        REP_ERR_GOTO_V1("Could not allocate sale.\n", alloc_err);
    if ((end = lseek (sale_fd, 0, SEEK_END)) == -1)
        REP_ERR_GOTO_V2 ("Could not seek sales.\n", seek_err);
    if (offset >= end)
        return NULL;
    if (lseek (sale_fd, SIZEOF_SALE, SEEK_SET) == -1)
        REP_ERR_GOTO_V2 ("Could not seek sales.\n", seek_err);
    if (read (sale_fd, s, SIZEOF_SALE) != SIZEOF_SALE)
        REP_ERR_GOTO_V1("Could not read sale.", read_err);
    return s;
read_err:
seek_err:
    free (s);
alloc_err:
    return NULL;
}

int sale_write_end (sale s, int sale_fd){
    if (lseek (sale_fd, 0, SEEK_END) == -1)
        REP_ERR_GOTO_V2 ("Error trying to seek file.\n", seek_err);
    if (write (sale_fd, s, SIZEOF_SALE) == -1)
        REP_ERR_GOTO_V2 ("Error trying to write to file.\n", write_err);
    return 0;
write_err:
seek_err:
    return -1;
}

stock_am_type stock_get (int id, int stock_fd){
    off_t offset = sizeof(stock_am_type) * id;
    stock_am_type curr = 0;
    if (lseek (stock_fd, offset, SEEK_SET) == -1)
        REP_ERR_GOTO_V2("Error trying to seek current stock.\n", seek_err);
    if (read (stock_fd, &curr, sizeof(stock_am_type)) != sizeof(stock_am_type))
        REP_ERR_GOTO_V2("Error trying to read current stock.\n", read_err);
    return curr;
read_err:
seek_err:
    return -1;
}

int stock_set (int id, stock_am_type amount, int stock_fd){
    if (id <= 0)
        return 0;
    off_t offset = sizeof(stock_am_type) * id;
    stock_am_type tmp = amount;
    if (lseek (stock_fd, offset, SEEK_SET) == -1)
        REP_ERR_GOTO_V2("Error trying to seek stock.\n", read_err);
    if (write (stock_fd, &tmp, sizeof(stock_am_type)) == -1)
        REP_ERR_GOTO_V2("Error trying to write current stock.\n", seek_err);
    return 0;
read_err:
seek_err:
    return -1;
}

int stock_add (int id, stock_am_type amount, int stock_fd){
    off_t offset = sizeof(stock_am_type) * id;
    stock_am_type curr = 0;
    if (lseek (stock_fd, offset, SEEK_SET) == -1)
        REP_ERR_GOTO_V2("Error trying to seek current stock.\n", seek_err);
    if (read (stock_fd, &curr, sizeof(stock_am_type)) == -1)
        REP_ERR_GOTO_V2("Error trying to read current stock.\n", read_err);
    if (lseek (stock_fd, offset, SEEK_SET) == -1)
        REP_ERR_GOTO_V2("Error trying to seek stock.\n", read_err);
    if (curr < -amount)
        REP_ERR_GOTO_V2 ("Not enough stock for sale.\n", stock_err);
    curr += amount;
    if (write (stock_fd, &curr, sizeof(stock_am_type)) == -1)
        REP_ERR_GOTO_V2("Error trying to write current stock.\n", seek_err);
    return 0;
stock_err:
read_err:
seek_err:
    return -1;
}

int sale_stock_update (sale s, int stock_fd, int sale_fd){
    if (stock_add (s->id, -s->sold_amount, stock_fd) == -1)
        goto err;
    if (sale_write_end (s, sale_fd) == -1)
        goto err;
    return 0;
err:
    return -1;
}


sale sale_read_next (int sale_fd){
    sale s;
    if ((s = calloc (1, SIZEOF_SALE)) == NULL)
        REP_ERR_GOTO_V2("Error trying to allocate sale.\n", alloc_err);
    if (read (sale_fd, s, SIZEOF_SALE) != SIZEOF_SALE)
        REP_ERR_GOTO_V2("Could not read full sale.\n", read_fail);
    return s;
read_fail:
    free (s);
alloc_err:
    return NULL;
}

void sale_add (sale dest, const sale source){
    dest->sold_amount += source->sold_amount;
    dest->total_value += source->total_value;
}

int sale_write (int sale_fd, const sale s){
    return ((write (sale_fd, s, SIZEOF_SALE) == SIZEOF_SALE)-1);
}

off_t sale_offset (size_t i){
    return i * SIZEOF_SALE;
}
