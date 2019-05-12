#ifndef __STOCK_H__
#define __STOCK_H__

#include <ctype.h>
#include "item.h"

typedef  int64_t  stock_t;

stock_t stock_rd  (iid_t id, int fd_stock);
int     stock_inc (iid_t id, stock_t stk, int fd_item, int fd_stock);
int     stock_wr  (iid_t id, stock_t stk, int fd_item, int fd_stock);

#endif
