#ifndef __SALE_H__
#define __SALE_H__

#include "item.h"
#include "stock.h"

typedef struct sale* sale;

iid_t   sale_id     (sale);
price_t sale_value  (sale);
stock_t sale_amount (sale);
size_t sale_size ();

sale sale_creat (iid_t id, price_t value, stock_t amount);
void sale_free (sale s);

sale sale_rd (uint64_t n, int fd_sale);
int  sale_wr (sale s, int fd_sale);

sale sale_rd_stdin ();
void sale_wr_stdout (sale s);

uint64_t no_of_sales (int fd_sale);

void sale_merge (sale s1, const sale s2);

#endif
