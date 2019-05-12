#include "stock.h"

stock_t stock_rd (iid_t id, int fd_stock){
    if (lseek(fd_stock, 0, SEEK_END) <= (off_t)(sizeof(stock_t) * id))
        return 0;
    stock_t tmp;
    pread(fd_stock, &tmp, sizeof(tmp), sizeof(tmp)*id);
    return tmp;
}

int stock_wr (iid_t id, stock_t stk, int fd_item, int fd_stock){
    if (id >= item_id_top(fd_item))
        return -1;
    if (stk < 0) return -1;
    return pwrite(fd_stock, &stk, sizeof(stk), sizeof(stk)*id);
}

int stock_inc (iid_t id, stock_t stk, int fd_item, int fd_stock){
    if (id >= item_id_top(fd_item))
        return -1;
    stock_t tmp = stock_rd(id, fd_stock) + stk;
    if (tmp < 0) return -1;
    return pwrite(fd_stock, &tmp, sizeof(tmp), sizeof(tmp)*id);
}
