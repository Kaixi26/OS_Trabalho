#include "cache.h"

#define CACHE_SIZE 100

typedef struct cache {
    cache_unit units[CACHE_SIZE];
    int fd_stock;
    int fd_item;
}* cache;

cache cache_creat (const char* stock_path, const char* item_path){
    cache tmp;
    if ((tmp = calloc (1, sizeof (struct cache))) == NULL)
        REP_ERR_GOTO_V2 ("Error allocating cache.\n", alloc_err);
    if ((tmp->fd_stock = open (stock_path, O_RDWR)) == -1)
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

cache_unit* cache_access (cache cach, id_type id){
    return &(cach->units[id % CACHE_SIZE]);
}

int cache_is_cached (cache cach, id_type id){
    return (cach->units[id % CACHE_SIZE].id == id);
}

void cache_load (cache cach, id_type id){
    cache_unit* tmp = cache_access(cach, id);
    if (!cache_is_cached (cach, id)){
        stock_set (tmp->id, tmp->stock, cach->fd_stock);
        tmp->id = id;
        tmp->stock = stock_get (id, cach->fd_stock);
        if (tmp->stock == -1)
            tmp->stock = 0;
        tmp->price = item_read_price(id, cach->fd_item);
    }
}

void cache_update_stock (cache cach, id_type id, stock_am_type diff){
    cache_load (cach, id);
    cache_unit* tmp = cache_access(cach, id);
    if (tmp->stock >= -diff)
        tmp->stock+=diff;
}

void cache_update_price (cache cach, id_type id){
    if (!cache_is_cached (cach, id))
        cache_access(cach,id)->price = item_read_price(id, cach->fd_item);
}

cache_unit cache_get (cache cach, id_type id){
    if (!cache_is_cached (cach, id))
        cache_load (cach, id);
    return cach->units[id % CACHE_SIZE];
}

id_type cache_get_maxid (cache cach){
    return item_amount (cach->fd_item);
}

void cache_save_all (cache cach){
    for (int i=0; i<CACHE_SIZE; i++){
        cache_unit* tmp = cache_access(cach, i);
        if (tmp->id > 0){
            stock_set (tmp->id, tmp->stock, cach->fd_stock);
        }
    }
}

void cache_free (cache cach){
    cache_save_all (cach);
    if (cach){
        close (cach->fd_stock);
        close (cach->fd_item);
        free (cach);
    }
}
