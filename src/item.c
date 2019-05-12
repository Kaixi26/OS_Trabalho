#include "item.h"
typedef off_t    n_ref_t;

typedef struct item_container {
    iid_t id;
    price_t price;
    n_len_t name_len;
    n_ref_t name_ref;
} item_container_t;

struct item {
    item_container_t ic;
    char* name;
};

typedef struct item* item;

item item_creat
(id_t id, price_t price, n_len_t name_len, n_ref_t name_ref, char* name){
    item it = calloc (1, sizeof (struct item));
    it->ic.id = id;
    it->ic.price = price;
    it->ic.name_len = name_len;
    it->ic.name_ref = name_ref;
    it->name = name;
    return it;
}

item item_creat_ic (item_container_t ic, char* name){
    return item_creat(ic.id, ic.price, ic.name_len, ic.name_ref, name);
}

void item_free (item it){
    if (it){
        free ((it)->name);
        free (it);
    }
}

iid_t item_id_top (int fd_item){
    iid_t ret = lseek (fd_item, 0, SEEK_END) / sizeof(item_container_t);
    return (ret >= 1) ? ret : 1;
}

n_len_t item_wasted_len (int fd_item_n){
    n_len_t curr;
    pread(fd_item_n, &curr, sizeof(curr), 0);
    return curr;
}

void item_wasted_len_inc (n_len_t n, int fd_item_n){
    n_len_t curr = item_wasted_len (fd_item_n) + n;
    pwrite(fd_item_n, &curr, sizeof(curr), 0);
}


n_len_t item_total_len (int fd_item_n){
    return lseek (fd_item_n, 0, SEEK_END) - sizeof(n_len_t);
}

n_ref_t item_wr_name (const char* name, int fd_item_n){
    n_ref_t ret = lseek (fd_item_n, 0, SEEK_END);
    write (fd_item_n, name, strlen(name));
    return ret;
}

char* item_rd_name (item_container_t ic, int fd_item_n){
    char* tmp = calloc (ic.name_len+1, sizeof(char));
    pread (fd_item_n, tmp, ic.name_len,ic.name_ref);
    return tmp;
}

item item_rd (iid_t id, int fd_item, int fd_item_n){
    if (id >= item_id_top(fd_item))
        return NULL;
    item_container_t tmp_ic;
    pread(fd_item, &tmp_ic, sizeof(tmp_ic), sizeof(tmp_ic) * id);
    char* tmp_name = item_rd_name(tmp_ic, fd_item_n);
    return item_creat_ic (tmp_ic, tmp_name);
}

price_t item_rd_price (iid_t id, int fd_item){
    if (id >= item_id_top(fd_item))
        return 0;
    item_container_t tmp_ic;
    pread(fd_item, &tmp_ic, sizeof(tmp_ic), sizeof(tmp_ic) * id);
    return tmp_ic.price;
}

void item_wr_new (price_t price, const char* name, int fd_item, int fd_item_n){
    item_container_t tmp;
    tmp.id = item_id_top (fd_item);
    tmp.price = price;
    tmp.name_len = strlen (name);
    tmp.name_ref = item_wr_name (name, fd_item_n);
    pwrite (fd_item, &tmp, sizeof(tmp), tmp.id * sizeof(tmp));
}

void item_ch_name (iid_t id, const char* name, int fd_item, int fd_item_n){
    if (id >= item_id_top(fd_item))
        return;
    item_container_t tmp;
    pread(fd_item, &tmp, sizeof(tmp), id * sizeof(tmp));
    item_wasted_len_inc (tmp.name_len, fd_item_n);
    tmp.name_len = strlen(name);
    tmp.name_ref = item_wr_name(name, fd_item_n);
    pwrite(fd_item, &tmp, sizeof(tmp), id * sizeof(tmp));
}

void item_ch_price (iid_t id, price_t price, int fd_item){
    if (id >= item_id_top(fd_item))
        return;
    item_container_t tmp;
    pread(fd_item, &tmp, sizeof(tmp), id * sizeof(tmp));
    tmp.price = price;
    pwrite(fd_item, &tmp, sizeof(tmp), id * sizeof(tmp));
}

price_t item_price (item it){
    return it->ic.price;
}

const char* item_name (item it){
    return it->name;
}
