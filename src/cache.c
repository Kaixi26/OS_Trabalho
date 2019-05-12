#include "cache.h"

#define CACHE_SIZE 100

typedef struct cache {
    cache_unit units[CACHE_SIZE];
    int fd_stock;
    int fd_item;
}* cache;

cache cache_creat (const char* stock_path, const char* item_path){
    cache tmp = calloc(1, sizeof(struct cache));
    tmp->fd_stock = open (stock_path, O_RDWR | O_CREAT);
    tmp->fd_item  = open (item_path, O_RDONLY);
    if (tmp->fd_stock == -1 || tmp->fd_item == -1) return NULL;
    return tmp;
}

cache_unit* cache_acess (cache cach, iid_t id){
    return &(cach->units[id % CACHE_SIZE]);
}

int cache_is_cached (cache cach, iid_t id){
    return (cach->units[id%CACHE_SIZE].id == id);
}

void cache_save_stock (cache cach, int index){
    cache_unit* cunit = cache_acess(cach, index);
    if (cunit->id <= 0) return;
    stock_wr(cunit->id, cunit->stock, cach->fd_item, cach->fd_stock);
}

void cache_free (cache cach){
    for (int i=0; i<CACHE_SIZE; i++)
        cache_save_stock (cach, i);
}

int cache_load (cache cach, iid_t id){
    if (cache_is_cached(cach, id)) return 0;
    if (id >= item_id_top(cach->fd_item)) return -1;
    cache_save_stock(cach, id);
    cache_unit* tmp = cache_acess(cach, id);
    tmp->id = id;
    tmp->price = item_rd_price (id, cach->fd_item);
    tmp->stock = stock_rd (id, cach->fd_stock);
    return 0;
}

void cache_reload_price (cache cach, iid_t id){
    if (!cache_is_cached(cach, id)) return;
    cache_unit* tmp = cache_acess(cach, id);
    tmp->price = item_rd_price (id, cach->fd_item);
}

int cache_inc_stock (cache cach, iid_t id, stock_t amount){
    if (cache_load(cach, id) == -1) return -1;
    cache_unit* tmp = cache_acess(cach, id);
    if (tmp->stock < -amount)
        return -1;
    tmp->stock += amount;
    return 0;
}

cache_unit cache_get (cache cach, iid_t id){
    if (cache_load(cach, id) == -1) return (cache_unit){-1,-1,-1};
    return *cache_acess(cach, id);
}

price_t cache_get_price (cache cach, iid_t id){
    if (cache_load(cach, id) == -1) return -1;
    return cache_acess(cach, id)->price;
}
