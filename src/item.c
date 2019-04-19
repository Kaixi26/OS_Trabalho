#include "item.h"

#define SIZEOF_PRICE sizeof (int)
#define SIZEOF_NAME_REF sizeof (off_t)
#define SIZEOF_NAME_LEN sizeof (int)
#define SIZEOF_ITEM_ENTRY (SIZEOF_PRICE + SIZEOF_NAME_REF + SIZEOF_NAME_LEN)

struct item {
  int id;
  int price;
  char* name;
};

typedef struct item* item;

item item_creat (int id, int price, const char* name){
  item it = calloc (1, sizeof (struct item));
  if (!it){
    fprintf (stderr, "Error trying to allocate item.\n");
    goto alloc_failure;
  }
  if (!(it->name = strdup (name))){
    fprintf (stderr, "Error trying to allocate item name.\n");
    goto name_alloc_failure;
  }
  it->id = id;
  it->price = price;
  return it;
 name_alloc_failure:
  free (it);
 alloc_failure:
  return NULL;
}

void item_dest (item* it){
  if (*it){
    free ((*it)->name);
    free (*it);
  }
}

int item_price (item it){
  return it->price;
}

const char* item_name (item it){
  return it->name;
}

int item_price_set (item it, int price){
  it->price = price;
  return 0;
}

int item_name_set (item it, const char* name){
  free (it->name);
  it->name = strdup (name);
  return 0;
}

int item_amount (int item_fd){
  int am;
  int tmp_item_fd;
  if ((tmp_item_fd = dup (item_fd)) == -1){
    fprintf (stderr, "Error trying to duplicate file descriptor.\n");
    goto dup_failure;
  }
  lseek (tmp_item_fd, 0, SEEK_SET);
  if ((read (tmp_item_fd, &am, sizeof(int)) != sizeof(int))){
    fprintf (stderr, "Error reading item amount.\n");
    goto read_failure;
  }
  close (tmp_item_fd);
  return am;
 read_failure:
  close (tmp_item_fd);
 dup_failure:
  return -1;
}

static char* item_read_name (int item_str_fd, int name_len, off_t name_ref){
  int tmp_item_str_fd;
  char name[name_len+1];
  if ((tmp_item_str_fd = dup (item_str_fd)) == -1){
    fprintf (stderr, "Error trying to duplicate file descriptor.\n");
    goto item_str_fd_dup_failure;
  }
  lseek (tmp_item_str_fd, name_ref, SEEK_SET);
  if (read (tmp_item_str_fd, &name, name_len) < name_len){
    fprintf (stderr, "Error trying to read item name.\n");
    goto name_read_failure;
  }
  return strdup (name);
 name_read_failure:
  close (tmp_item_str_fd);
 item_str_fd_dup_failure:
  return NULL;
}

item item_read (int id, int item_fd, int item_str_fd){
  int tmp_item_fd;
  off_t offset = id * SIZEOF_ITEM_ENTRY;
  int price;
  int name_len;
  off_t name_ref;
  char* name;
  item it;
  if ((tmp_item_fd = dup (item_fd)) == -1){
    fprintf (stderr, "Error trying to duplicate file descriptor.\n");
    goto item_fd_dup_failure;
  }
  if (lseek(tmp_item_fd, 0, SEEK_END) < lseek (tmp_item_fd, offset, SEEK_SET)){
    fprintf (stderr, "Error reading item: id is too high.\n");
    goto nonexistant_id_failure;
  }
  if (read (tmp_item_fd, &price, SIZEOF_PRICE) < SIZEOF_PRICE){
    fprintf (stderr, "Error reading item: couldn't parse price.\n");
    goto nonexistant_id_failure;
  }
  if (read (tmp_item_fd, &name_len, SIZEOF_NAME_LEN) < SIZEOF_NAME_LEN){
    fprintf (stderr, "Error reading item: couldn't parse name length.\n");
    goto nonexistant_id_failure;
  }
  if (read (tmp_item_fd, &name_ref, SIZEOF_NAME_REF) < SIZEOF_NAME_REF){
    fprintf (stderr, "Error reading item: couldn't parse name reference.\n");
    goto nonexistant_id_failure;
  }
  if (!(name = item_read_name (item_str_fd, name_len, name_ref))){
    goto name_read_failure;
  }
  if (!(it = item_creat (id, price, name))){
    goto item_creat_failure;
  }
  free (name);
  close (tmp_item_fd);
  return it;
 item_creat_failure:
  free (name);
 name_read_failure:
 nonexistant_id_failure:
  close (tmp_item_fd);
 item_fd_dup_failure:
  return NULL;
}


static int item_write_name (int item_name_fd, char* name, int* name_len, off_t* name_ref){
  int tmp_item_name_fd;
  if ((tmp_item_name_fd = dup (item_name_fd)) == -1){
    fprintf (stderr, "Error trying to duplicate file descriptor.\n");
    goto item_name_fd_dup_failure;
  }
  *name_ref = lseek (tmp_item_name_fd, 0, SEEK_END);
  if (write (tmp_item_name_fd, name, *name_len = strlen (name)) == -1){
    fprintf (stderr, "Error trying to write item name.\n");
    goto item_name_write_failure;
  }
  close (tmp_item_name_fd);
  return 0;
 item_name_write_failure:
  close (tmp_item_name_fd);
 item_name_fd_dup_failure:
  return -1;
}

int item_write (item it, int item_fd, int item_name_fd){
  int tmp_item_fd;
  off_t offset = it->id * SIZEOF_ITEM_ENTRY;
  int name_len;
  off_t name_ref;
  if ((tmp_item_fd = dup (item_fd)) == -1){
    goto item_fd_dup_failure;
  }
  if (lseek (tmp_item_fd, offset, SEEK_SET) != offset){
    fprintf (stderr, "Error writing item: invalid id.\n");
    goto offset_failure;
  }
  if (item_write_name (item_name_fd, it->name, &name_len, &name_ref) == -1){
    goto write_name_failure;
  }
  if (write (tmp_item_fd, &it->price, SIZEOF_PRICE) == -1){
    goto write_failure;
  }
  if (write (tmp_item_fd, &name_len, SIZEOF_NAME_LEN) == -1){
    goto write_failure;
  }
  if (write (tmp_item_fd, &name_ref, SIZEOF_NAME_REF) == -1){
    goto write_failure;
  }
  close (tmp_item_fd);
  return 0;
 write_failure:
  fprintf (stderr, "Error writing item.\n");
 write_name_failure:
 offset_failure:
  close (tmp_item_fd);
 item_fd_dup_failure:
  return -1;
}

static int item_inc (int item_fd){
  int curr;
  int tmp_item_fd;
  if ((tmp_item_fd = dup (item_fd)) == -1){
    fprintf (stderr, "Error trying to duplicate file descriptor.\n");
    goto dup_failure;
  }
  lseek (tmp_item_fd, 0, SEEK_SET);
  if (read (item_fd, &curr, sizeof(int)) != sizeof(int)){
    fprintf (stderr, "Error trying to read item amount.\n");
    goto read_failure;
  }
  lseek (tmp_item_fd, 0, SEEK_SET);
  curr++;
  if (write (item_fd, &curr, sizeof(int)) != sizeof(int)){
    fprintf (stderr, "Error trying to write new item amount.\n");
    goto write_failure;
  }
  close (tmp_item_fd);
  return 0;
 write_failure:
 read_failure:
  close (tmp_item_fd);
 dup_failure:
  return -1;
}


int item_new_append (int price, const char* name, int item_fd, int item_name_fd){
  item it;
  int curr, id;
  if ((curr = item_amount (item_fd)) == -1){
    goto read_amount_failure;
  }
  id = curr + 1;
  if (!(it = item_creat (id, price, name))){
    goto create_failure;
  }
  if (item_write (it, item_fd, item_name_fd) == -1){
    goto write_failure;
  }
  if (item_inc (item_fd) == -1){
    goto inc_failure;
  }
  item_dest (&it);
  return id;
 inc_failure:
 write_failure:
  item_dest (&it);
 create_failure:
 read_amount_failure:
  return -1;
}

int item_fill_id0 (int item_fd){
  int tmp_item_fd;
  int amount = 0;
  char wr[SIZEOF_ITEM_ENTRY] = {0};
  strncpy (wr, (char*)&amount, sizeof(int));
  if ((tmp_item_fd = dup (item_fd)) == -1){
    fprintf (stderr, "Error duplicating file descriptor.\n");
    goto dup_failure;
  }
  lseek (tmp_item_fd, 0, SEEK_SET);
  if (write (tmp_item_fd, wr, SIZEOF_ITEM_ENTRY) != SIZEOF_ITEM_ENTRY){
    fprintf (stderr, "Error writing id0 item.\n");
    goto write_failure;
  }
  close (tmp_item_fd);
  return 0;
 write_failure:
  close (tmp_item_fd);
 dup_failure:
  return -1;
}
