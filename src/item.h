#ifndef __ITEM__
#define __ITEM__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "debug.h"

typedef struct item* item;
typedef  int64_t  price_type;
typedef  int64_t  id_type;
typedef  int32_t  item_amnt_type;

id_type item_id (item it);
price_type item_price (item it);
const char* item_name (item it);
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

#endif
