#include "item.h"
#include "sale.h"
#include "files.h"
#include "debug.h"
#include "stock.h"

typedef struct cache_unit {
    iid_t id;
    price_t price;
    stock_t stock;
} cache_unit;

typedef struct cache* cache;

cache cache_creat (const char* stock_path, const char* item_path);
void cache_free (cache cach);
int cache_inc_stock (cache cach, iid_t id, stock_t amount);
cache_unit cache_get (cache cach, iid_t id);
price_t cache_get_price (cache cach, iid_t id);
void cache_reload_price (cache cach, iid_t id);
