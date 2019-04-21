#ifndef __SALE__
#define __SALE__

#include "item.h"
#include "debug.h"

typedef struct sale* sale;
typedef ssize_t stock;

sale sale_creat (int id, item_price_type total_value, int sold_amount);
void sale_dest (sale* s);
int sale_id (sale s);
item_price_type sale_total (sale s);
int sale_sold (sale s);

sale sale_read (size_t i, int sale_fd);

stock stock_get (int id, int stock_fd);
int stock_add (int id, stock amount, int stock_fd);
int sale_stock_update (sale s, int stock_fd, int sale_fd);

#endif
