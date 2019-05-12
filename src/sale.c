#include "sale.h"


struct sale {
    iid_t id;
    price_t value;
    stock_t amount;
};

typedef struct sale* sale;

iid_t sale_id (sale s){
    return s->id;
}

price_t sale_value (sale s){
    return s->value;
}

stock_t sale_amount (sale s){
    return s->amount;
}

sale sale_creat (iid_t id, price_t value, stock_t amount){
    sale s = calloc(1, sizeof(struct sale));
    s->id = id;
    s->value = value;
    s->amount = amount;
    return s;
}

void sale_free (sale s){
    if (s)
        free(s);
}

sale sale_rd (uint64_t n, int fd_sale){
    off_t offset = n * sizeof(struct sale);
    if (lseek(fd_sale, 0, SEEK_END) < (off_t)(offset + sizeof(struct sale)))
        return NULL;
    sale tmp = calloc(1, sizeof(struct sale));
    pread(fd_sale, tmp, sizeof(struct sale), offset);
    return tmp;
}

sale sale_rd_stdin (){
    sale tmp = calloc(1, sizeof(struct sale));
    if (read(STDIN_FILENO, tmp, sizeof(struct sale)) != sizeof(struct sale)){
        free (tmp);
        return NULL;
    }
    return tmp;
}

int sale_wr (sale s, int fd_sale){
    lseek(fd_sale, 0, SEEK_END);
    return write(fd_sale, s, sizeof(struct sale));
}

void sale_wr_stdout (sale s){
    write(STDOUT_FILENO, s, sizeof(struct sale));
}

void sale_merge (sale s1, const sale s2){
    if (s1->id != s2->id) return;
    s1->value += s2->value;
    s1->amount += s2->amount;
}

uint64_t no_of_sales (int fd_sale){
    return lseek(fd_sale, 0, SEEK_END) / sizeof(struct sale);
}

size_t sale_size(){
    return sizeof(struct sale);
}
