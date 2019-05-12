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

/*
id_type item_id (item it);
price_type item_price (item it);
const char* item_name (item it);
int item_name_len (item it);
price_type item_read_price (int id, int item_fd);

int item_price_set (item it, price_type price);
int item_name_set (item it, const char* name);

double item_price_to_double (price_type price);
price_type double_to_item_price (double price);

void item_free (item it);

int item_fill_id0 (int item_fd);
item_amnt_type item_amount (int item_fd);
item item_read (id_type id, int item_fd, int item_str_fd);
price_type item_read_price (int id, int item_fd);
int item_write (item it, int item_fd, int item_name_fd);

int item_new_append (price_type price, const char* name, int item_fd, int item_name_fd);
void add_unused_item_name (uint64_t n, int item_name_fd);

#endif
*/
