#ifndef __ITEM__
#define __ITEM__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "debug.h"

typedef struct item* item;
typedef size_t item_price_type;

item_price_type item_price (item it);
const char* item_name (item it);

int item_price_set (item it, item_price_type price);
int item_name_set (item it, const char* name);

double item_price_to_double (item_price_type price);
item_price_type double_to_item_price (double price);

void item_dest (item* it);

int item_fill_id0 (int item_fd);
item item_read (int id, int item_fd, int item_str_fd);
int item_write (item it, int item_fd, int item_name_fd);

int item_new_append (item_price_type price, const char* name, int item_fd, int item_name_fd);

#endif
