#include "item.h"
#include "sale.h"
#include "debug.h"

typedef struct cache_unit {
    price_type price;
    stock_am_type stock;
} cache_unit;

typedef struct cache* cache;

cache cache_creat (const char* stock_path, const char* item_path);
void cache_free (cache cach);
cache_unit cache_get (cache cach, id_type id);
id_type cache_get_maxid (cache cach);
