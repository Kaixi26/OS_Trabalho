#ifndef __ITEM__
#define __ITEM__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct item* item;

int item_price (item it);
const char* item_name (item it);

void item_dest (item* it);

int item_fill_id0 (int item_fd);
item item_read (int id, int item_fd, int item_str_fd);
int item_new_append (int price, char* name, int item_fd, int item_name_fd);

#endif
