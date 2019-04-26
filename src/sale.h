#ifndef __SALE__
#define __SALE__

#include "item.h"
#include "debug.h"

typedef struct sale* sale;
typedef ssize_t stock_am_type;

sale sale_creat (int id, price_type total_value, int sold_amount);
void sale_free (sale s);
int sale_id (sale s);
price_type sale_total (sale s);
int sale_sold (sale s);

sale sale_read (size_t i, int sale_fd);
sale sale_read_next (int sale_fd);
void sale_add (sale dest, const sale source);
int sale_write (int sale_fd, const sale s);

stock_am_type stock_get (int id, int stock_fd);
/* ID's existance should be checked before trying to add stock */
int stock_add (int id, stock_am_type amount, int stock_fd);
int sale_stock_update (sale s, int stock_fd, int sale_fd);

#endif
