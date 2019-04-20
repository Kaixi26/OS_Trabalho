#ifndef __ITEM__
#define __ITEM__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "debug.h"

typedef struct item* item;

int item_price (item it);
const char* item_name (item it);

int item_price_set (item it, int price);
int item_name_set (item it, const char* name);

void item_dest (item* it);

int item_fill_id0 (int item_fd);
item item_read (int id, int item_fd, int item_str_fd);
int item_write (item it, int item_fd, int item_name_fd);

int item_new_append (int price, const char* name, int item_fd, int item_name_fd);

#endif
