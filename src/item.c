#include "item.h"

#define SIZEOF_PRICE sizeof (item_price_type)
#define SIZEOF_NAME_REF sizeof (off_t)
#define SIZEOF_NAME_LEN sizeof (int)
#define SIZEOF_ITEM_ENTRY (SIZEOF_PRICE + SIZEOF_NAME_REF + SIZEOF_NAME_LEN)

struct item {
    int id;
    item_price_type price;
    char* name;
};

typedef struct item* item;

item item_creat (int id, item_price_type price, const char* name){
    item it = calloc (1, sizeof (struct item));
    if (!it)
        REP_ERR_GOTO_V2 ("Error trying to allocate item.\n", alloc_err);
    if (!(it->name = strdup (name)))
        REP_ERR_GOTO_V2 ("Error trying to allocate item name.\n", name_alloc_error);
    it->id = id;
    it->price = price;
    return it;
name_alloc_error:
    free (it);
alloc_err:
    return NULL;
}

void item_dest (item* it){
    if (*it){
        free ((*it)->name);
        free (*it);
    }
}

item_price_type item_price (item it){
    return it->price;
}

const char* item_name (item it){
    return it->name;
}

int item_price_set (item it, item_price_type price){
    it->price = price;
    return 0;
}

int item_name_set (item it, const char* name){
    free (it->name);
    it->name = strdup (name);
    return 0;
}

double item_price_to_double (item_price_type price){
    return ((double)price)/100;
}

item_price_type double_to_item_price (double price){
    return (item_price_type)(round(price*100));
}

int item_amount (int item_fd){
    int am;
    int tmp_item_fd;
    if ((tmp_item_fd = dup (item_fd)) == -1)
        REP_ERR_GOTO_V2 ("Error trying to duplicate file descriptor.\n", dup_err);
    lseek (tmp_item_fd, 0, SEEK_SET);
    if ((read (tmp_item_fd, &am, sizeof(int)) != sizeof(int)))
        REP_ERR_GOTO_V2 ("Error reading item amount.\n", read_err);
    close (tmp_item_fd);
    return am;
read_err:
    close (tmp_item_fd);
dup_err:
    return -1;
}

static char* item_read_name (int item_str_fd, int name_len, off_t name_ref){
    int tmp_item_str_fd;
    char name[name_len+1];
    if ((tmp_item_str_fd = dup (item_str_fd)) == -1)
        REP_ERR_GOTO_V2 ("Error trying to duplicate file descriptor.\n", item_str_fd_dup_err);
    lseek (tmp_item_str_fd, name_ref, SEEK_SET);
    if (read (tmp_item_str_fd, &name, name_len) < name_len)
        REP_ERR_GOTO_V2 ("Error trying to read item name.\n", name_read_err);
    name[name_len] = 0;
    return strdup (name);
name_read_err:
    close (tmp_item_str_fd);
item_str_fd_dup_err:
    return NULL;
}

item item_read (int id, int item_fd, int item_str_fd){
    int tmp_item_fd;
    off_t offset = id * SIZEOF_ITEM_ENTRY;
    item_price_type price;
    int name_len;
    off_t name_ref;
    char* name;
    item it;
    if ((tmp_item_fd = dup (item_fd)) == -1)
        REP_ERR_GOTO_V2 ("Error trying to duplicate file descriptor.\n", item_fd_dup_err);
    if (lseek(tmp_item_fd, 0, SEEK_END) < lseek (tmp_item_fd, offset, SEEK_SET))
        REP_ERR_GOTO_V2 ("Error reading item: id is too high.\n", nonexistant_id_err);
    if (read (tmp_item_fd, &price, SIZEOF_PRICE) < SIZEOF_PRICE)
        REP_ERR_GOTO_V2 ("Error reading item: couldn't parse price.\n", nonexistant_id_err);
    if (read (tmp_item_fd, &name_len, SIZEOF_NAME_LEN) < SIZEOF_NAME_LEN)
        REP_ERR_GOTO_V2 ("Error reading item: couldn't parse name length.\n", nonexistant_id_err);
    if (read (tmp_item_fd, &name_ref, SIZEOF_NAME_REF) < SIZEOF_NAME_REF)
        REP_ERR_GOTO_V2 ("Error reading item: couldn't parse name reference.\n", nonexistant_id_err);
    if (!(name = item_read_name (item_str_fd, name_len, name_ref))){
        goto name_read_err;
    }
    if (!(it = item_creat (id, price, name))){
        goto item_creat_err;
    }
    free (name);
    close (tmp_item_fd);
    return it;
item_creat_err:
    free (name);
name_read_err:
nonexistant_id_err:
    close (tmp_item_fd);
item_fd_dup_err:
    return NULL;
}


static int item_write_name (int item_name_fd, char* name, int* name_len, off_t* name_ref){
    int tmp_item_name_fd;
    if ((tmp_item_name_fd = dup (item_name_fd)) == -1)
        REP_ERR_GOTO_V2 ("Error trying to duplicate file descriptor.\n", item_name_fd_dup_err);
    *name_ref = lseek (tmp_item_name_fd, 0, SEEK_END);
    if (write (tmp_item_name_fd, name, *name_len = strlen (name)) == -1)
        REP_ERR_GOTO_V2 ("Error trying to write item name.\n", item_name_write_err);
    close (tmp_item_name_fd);
    return 0;
item_name_write_err:
    close (tmp_item_name_fd);
item_name_fd_dup_err:
    return -1;
}

int item_write (item it, int item_fd, int item_name_fd){
    int tmp_item_fd;
    off_t offset = it->id * SIZEOF_ITEM_ENTRY;
    int name_len;
    off_t name_ref;
    if ((tmp_item_fd = dup (item_fd)) == -1)
        goto item_fd_dup_err;
    if (lseek (tmp_item_fd, offset, SEEK_SET) != offset)
        REP_ERR_GOTO_V2 ("Error writing item: invalid id.\n", offset_err);
    if (item_write_name (item_name_fd, it->name, &name_len, &name_ref) == -1)
        goto write_name_err;
    if (write (tmp_item_fd, &it->price, SIZEOF_PRICE) == -1)
        goto write_err;
    if (write (tmp_item_fd, &name_len, SIZEOF_NAME_LEN) == -1)
        goto write_err;
    if (write (tmp_item_fd, &name_ref, SIZEOF_NAME_REF) == -1)
        goto write_err;
    close (tmp_item_fd);
    return 0;
write_err:
    REPORT_ERROR_V2 ("Error writing item.\n");
write_name_err:
offset_err:
    close (tmp_item_fd);
item_fd_dup_err:
    return -1;
}

static int item_inc (int item_fd){
    int curr;
    int tmp_item_fd;
    if ((tmp_item_fd = dup (item_fd)) == -1)
        REP_ERR_GOTO_V2 ("Error trying to duplicate file descriptor.\n", dup_err);
    lseek (tmp_item_fd, 0, SEEK_SET);
    if (read (item_fd, &curr, sizeof(int)) != sizeof(int))
        REP_ERR_GOTO_V2 ("Error trying to read item amount.\n", read_err);
    lseek (tmp_item_fd, 0, SEEK_SET);
    curr++;
    if (write (item_fd, &curr, sizeof(int)) != sizeof(int))
        REP_ERR_GOTO_V2 ("Error trying to write new item amount.\n", write_err);
    close (tmp_item_fd);
    return 0;
write_err:
read_err:
    close (tmp_item_fd);
dup_err:
    return -1;
}


int item_new_append (item_price_type price, const char* name, int item_fd, int item_name_fd){
    item it;
    int curr, id;
    if ((curr = item_amount (item_fd)) == -1)
        goto read_amount_err;
    id = curr + 1;
    if (!(it = item_creat (id, price, name)))
        goto create_err;
    if (item_write (it, item_fd, item_name_fd) == -1)
        goto write_err;
    if (item_inc (item_fd) == -1)
        goto inc_err;
    item_dest (&it);
    return id;
inc_err:
write_err:
    item_dest (&it);
create_err:
read_amount_err:
    return -1;
}

int item_fill_id0 (int item_fd){
    int tmp_item_fd;
    int amount = 0;
    char wr[SIZEOF_ITEM_ENTRY] = {0};
    strncpy (wr, (char*)&amount, sizeof(int));
    if ((tmp_item_fd = dup (item_fd)) == -1)
        REP_ERR_GOTO_V2 ("Error duplicating file descriptor.\n", dup_err);
    lseek (tmp_item_fd, 0, SEEK_SET);
    if (write (tmp_item_fd, wr, SIZEOF_ITEM_ENTRY) != SIZEOF_ITEM_ENTRY)
        REP_ERR_GOTO_V2 ("Error writing id0 item.\n", write_err);
    close (tmp_item_fd);
    return 0;
write_err:
    close (tmp_item_fd);
dup_err:
    return -1;
}
