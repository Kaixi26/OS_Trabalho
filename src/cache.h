#include "item.h"
#include "sale.h"
#include "files.h"
#include "debug.h"
#include "stock.h"

// pedaço de memória da cache
typedef struct cache_unit {
    iid_t id;
    price_t price;
    stock_t stock;
} cache_unit;

// estrutura que armazena a cache
typedef struct cache* cache;

// aloca o espaço e descritores para a cache
cache cache_creat (const char* stock_path, const char* item_path);
// liberta o espaço e descritores alocados para a cache
void cache_free (cache cach);

// incrementa o stock de um determinado artigo
int cache_inc_stock (cache cach, iid_t id, stock_t amount);
// devolve a cache_unit correspondente ao artigo
cache_unit cache_get (cache cach, iid_t id);
// devolve o preço de um artigo
price_t cache_get_price (cache cach, iid_t id);
// recarrega o preço de um artigo
void cache_reload_price (cache cach, iid_t id);
