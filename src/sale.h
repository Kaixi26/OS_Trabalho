#ifndef __SALE__
#define __SALE__

#include "item.h"
#include "debug.h"

typedef struct sale* sale;

int stock_fd_write_empty (int maxid, int stock_fd);
int sale_stock_update (sale s, int stock_fd, int sale_fd);

#endif
