#ifndef __STOCK_H__
#define __STOCK_H__

#include <ctype.h>
#include "item.h"

// tipo do stock de artigos
typedef  int64_t  stock_t;

// lê o stock de um artigo
stock_t stock_rd  (iid_t id, int fd_stock);
// incrementa o stock de um artigo no ficheiro
int     stock_inc (iid_t id, stock_t stk, int fd_item, int fd_stock);
// escreve o stock de um artigo para o ficheiro
int     stock_wr  (iid_t id, stock_t stk, int fd_item, int fd_stock);

#endif
