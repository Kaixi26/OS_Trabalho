#ifndef __ITEM__
#define __ITEM__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "debug.h"
#include <errno.h>

// Estrutura que guarda um artigo
typedef struct item* item;
// tipo do preço de um artigo
typedef  double   price_t;
// tipo do id de um artigo
typedef  int64_t  iid_t;
// tipo do tamanho do nome de um artigo
typedef  int64_t  n_len_t;

// retorna o preço do artigo dado, não pode ser NULL
price_t     item_price (item);
// retorna o nome do artigo dado, não pode ser NULL
const char* item_name  (item);

// retorna o id do próximo artigo a ser adicionado
iid_t item_id_top (int fd_item);

// lê um artigo (usa seek)
item  item_rd       (iid_t id, int fd_item, int fd_item_n);
// lê o preço de um artigo
price_t item_rd_price (iid_t id, int fd_item);
// escreve o artigo para os ficheiros dados
void  item_wr_new   (price_t price, const char* name, int fd_item, int fd_item_n);
// altera o preço do artigo nos artigos dados
void  item_ch_price (iid_t id, price_t price, int fd_item);
// altera o nome do artigo nos ficheiros dados
void  item_ch_name  (iid_t id, const char* name, int fd_item, int fd_item_n);

// calcula o total de bytes que estão a ser desperdiçados em STRINGS
n_len_t item_wasted_len (int fd_item_n);
// calcula o total de bytes que estão a ser usados em STRINGS
n_len_t item_total_len  (int fd_item_n);

#endif
