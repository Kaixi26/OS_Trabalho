#include "cache.h"

typedef struct cache {
    int fd_stock;
    int fd_item;
}* cache;

cache cache_creat (const char* stock_path, const char* item_path){
    cache tmp;
    if ((tmp = calloc (1, sizeof (struct cache))) == NULL)
        REP_ERR_GOTO_V2 ("Error allocating cache.\n", alloc_err);
    if ((tmp->fd_stock = open (stock_path, O_RDONLY)) == -1)
        REP_ERR_GOTO_V2 ("Error opening stock file for cache.\n", stock_open_err);
    if ((tmp->fd_item = open (item_path, O_RDONLY)) == -1)
        REP_ERR_GOTO_V2 ("Error opening item file for cache.\n", item_open_err);
    return tmp;
item_open_err:
    close (tmp->fd_stock);
stock_open_err:
    free (tmp);
alloc_err:
    return NULL;
}

cache_unit cache_get (cache cach, id_type id){
    cache_unit c_unit;
    c_unit.stock = stock_get (id, cach->fd_stock);
    c_unit.price = item_read_price(id, cach->fd_item);
    return c_unit;
}

void cache_free (cache cach){
    if (cach){
        close (cach->fd_stock);
        close (cach->fd_item);
        free (cach);
    }
}
