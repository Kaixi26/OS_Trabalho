#include "item.h"

#define SIZEOF_PRICE sizeof (price_type)
#define SIZEOF_NAME_REF sizeof (name_ref_type)
#define SIZEOF_NAME_LEN sizeof (name_len_type)
#define SIZEOF_ITEM_ENTRY (SIZEOF_PRICE + SIZEOF_NAME_REF + SIZEOF_NAME_LEN)

typedef uint32_t name_len_type;
typedef uint32_t name_ref_type;

struct item {
    id_type id;
    price_type price;
    name_len_type name_len;
    char* name;
};

typedef struct item* item;

item item_creat (id_type id, price_type price, name_len_type name_len, const char* name){
    item it = calloc (1, sizeof (struct item));
    if (!it)
        REP_ERR_GOTO_V2 ("Error trying to allocate item.\n", alloc_err);
    if (!(it->name = strdup (name)))
        REP_ERR_GOTO_V2 ("Error trying to allocate item name.\n", name_alloc_error);
    it->id = id;
    it->price = price;
    it->name_len = name_len;
    return it;
name_alloc_error:
    free (it);
alloc_err:
    return NULL;
}

void item_free (item it){
    if (it){
        free ((it)->name);
        free (it);
    }
}

id_type item_id (item it){
    return it->id;
}

price_type item_price (item it){
    return it->price;
}

const char* item_name (item it){
    return it->name;
}

int item_price_set (item it, price_type price){
    it->price = price;
    return 0;
}

int item_name_set (item it, const char* name){
    free (it->name);
    it->name = strdup (name);
    it->name_len = strlen (it->name);
    return 0;
}

double item_price_to_double (price_type price){
    return ((double)price)/100;
}

price_type double_to_item_price (double price){
    return (price_type)(round(price*100));
}

item_amnt_type item_amount (int item_fd){
    item_amnt_type am;
    lseek (item_fd, 0, SEEK_SET);
    if ((read (item_fd, &am, sizeof(item_amnt_type)) != sizeof(item_amnt_type)))
        REP_ERR_GOTO_V2 ("Error reading item amount.\n", read_err);
    return am;
read_err:
    return -1;
}

static char* item_read_name (int item_str_fd, int name_len, off_t name_ref){
    char name[name_len+1];
    lseek (item_str_fd, name_ref, SEEK_SET);
    if (read (item_str_fd, &name, name_len) < name_len)
        REP_ERR_GOTO_V2 ("Error trying to read item name.\n", name_read_err);
    name[name_len] = 0;
    return strdup (name);
name_read_err:
    return NULL;
}

price_type item_read_price (int id, int item_fd){
    off_t offset = id * SIZEOF_ITEM_ENTRY;
    price_type price;
    if (lseek(item_fd, 0, SEEK_END) < lseek (item_fd, offset, SEEK_SET))
        REP_ERR_GOTO_V2 ("Error reading item: id is too high.\n", nonexistant_id_err);
    if (read (item_fd, &price, SIZEOF_PRICE) < (int)SIZEOF_PRICE)
        REP_ERR_GOTO_V2 ("Error reading item: couldn't parse price.\n", nonexistant_id_err);
    return price;
nonexistant_id_err:
    return -1;

}

item item_read (id_type id, int item_fd, int item_str_fd){
    off_t offset = id * SIZEOF_ITEM_ENTRY;
    price_type price;
    int name_len;
    off_t name_ref;
    char* name;
    item it;
    if (lseek(item_fd, 0, SEEK_END) < lseek (item_fd, offset, SEEK_SET))
        REP_ERR_GOTO_V2 ("Error reading item: id is too high.\n", nonexistant_id_err);
    if (read (item_fd, &price, SIZEOF_PRICE) < (int)SIZEOF_PRICE)
        REP_ERR_GOTO_V2 ("Error reading item: couldn't parse price.\n", nonexistant_id_err);
    if (read (item_fd, &name_len, SIZEOF_NAME_LEN) < (int)SIZEOF_NAME_LEN)
        REP_ERR_GOTO_V2 ("Error reading item: couldn't parse name length.\n", nonexistant_id_err);
    if (read (item_fd, &name_ref, SIZEOF_NAME_REF) < (int)SIZEOF_NAME_REF)
        REP_ERR_GOTO_V2 ("Error reading item: couldn't parse name reference.\n", nonexistant_id_err);
    if (!(name = item_read_name (item_str_fd, name_len, name_ref))){
        goto name_read_err;
    }
    if (!(it = item_creat (id, price, name_len, name))){
        goto item_creat_err;
    }
    free (name);
    return it;
item_creat_err:
    free (name);
name_read_err:
nonexistant_id_err:
    return NULL;
}


static int item_write_name (int item_name_fd, item it, off_t* name_ref){
    *name_ref = lseek (item_name_fd, 0, SEEK_END);
    if (write (item_name_fd, it->name, it->name_len) == -1)
        REP_ERR_GOTO_V2 ("Error trying to write item name.\n", item_name_write_err);
    return 0;
item_name_write_err:
    return -1;
}

int item_write (item it, int item_fd, int item_name_fd){
    off_t offset = it->id * SIZEOF_ITEM_ENTRY;
    off_t name_ref;
    if (lseek (item_fd, offset, SEEK_SET) != offset)
        REP_ERR_GOTO_V2 ("Error writing item: invalid id.\n", offset_err);
    if (item_write_name (item_name_fd, it, &name_ref) == -1)
        goto write_name_err;
    if (write (item_fd, &it->price, SIZEOF_PRICE) == -1)
        goto write_err;
    if (write (item_fd, &(it->name_len), SIZEOF_NAME_LEN) == -1)
        goto write_err;
    if (write (item_fd, &name_ref, SIZEOF_NAME_REF) == -1)
        goto write_err;
    return 0;
write_err:
    REPORT_ERROR_V2 ("Error writing item.\n");
write_name_err:
offset_err:
    return -1;
}

static int item_inc (int item_fd){
    int curr;
    lseek (item_fd, 0, SEEK_SET);
    if (read (item_fd, &curr, sizeof(int)) != sizeof(int))
        REP_ERR_GOTO_V2 ("Error trying to read item amount.\n", read_err);
    lseek (item_fd, 0, SEEK_SET);
    curr++;
    if (write (item_fd, &curr, sizeof(int)) != sizeof(int))
        REP_ERR_GOTO_V2 ("Error trying to write new item amount.\n", write_err);
    return 0;
write_err:
read_err:
    return -1;
}


int item_new_append (price_type price, const char* name, int item_fd, int item_name_fd){
    item it;
    int curr, id;
    if ((curr = item_amount (item_fd)) == -1)
        goto read_amount_err;
    id = curr + 1;
    if (!(it = item_creat (id, price, strlen(name), name)))
        goto create_err;
    if (item_write (it, item_fd, item_name_fd) == -1)
        goto write_err;
    if (item_inc (item_fd) == -1)
        goto inc_err;
    item_free (it);
    return id;
inc_err:
write_err:
    item_free (it);
create_err:
read_amount_err:
    return -1;
}

int item_fill_id0 (int item_fd){
    int amount = 0;
    char wr[SIZEOF_ITEM_ENTRY] = {0};
    strncpy (wr, (char*)&amount, sizeof(int));
    lseek (item_fd, 0, SEEK_SET);
    if (write (item_fd, wr, SIZEOF_ITEM_ENTRY) != SIZEOF_ITEM_ENTRY)
        REP_ERR_GOTO_V2 ("Error writing id0 item.\n", write_err);
    return 0;
write_err:
    return -1;
}
