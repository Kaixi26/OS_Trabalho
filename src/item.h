#ifndef __ITEM__
#define __ITEM__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "debug.h"
#include <errno.h>

typedef struct item* item;
typedef  double   price_t;
typedef  int64_t  iid_t;
typedef  int64_t  n_len_t;

price_t     item_price (item);
const char* item_name  (item);

iid_t item_id_top (int fd_item);

item  item_rd       (iid_t id, int fd_item, int fd_item_n);
price_t item_rd_price (iid_t id, int fd_item);
void  item_wr_new   (price_t price, const char* name, int fd_item, int fd_item_n);
void  item_ch_name  (iid_t id, const char* name, int fd_item, int fd_item_n);
void  item_ch_price (iid_t id, price_t price, int fd_item);

n_len_t item_wasted_len (int fd_item_n);
n_len_t item_total_len  (int fd_item_n);

#endif
