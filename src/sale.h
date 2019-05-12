#ifndef __SALE_H__
#define __SALE_H__

#include "item.h"
#include "stock.h"

// estrutura que armazena as vendas
typedef struct sale* sale;

// retorna o tamanho da estrutura de vendas
size_t sale_size ();

// retorna o id correspondente à venda
iid_t   sale_id     (sale);
// retorna o preço total da venda
price_t sale_value  (sale);
// retorna o stock vendido
stock_t sale_amount (sale);

// aloca o espaço para um venda
sale sale_creat (iid_t id, price_t value, stock_t amount);
// liberta o espaço reservado para a venda
void sale_free (sale s);

// lê uma venda do ficheiro de vendas
sale sale_rd (uint64_t n, int fd_sale);
// escreve uma venda no fim do ficheiro de vendas
int  sale_wr (sale s, int fd_sale);

// lê uma venda do stdin
sale sale_rd_stdin ();
// escreve uma venda para o stdout
void sale_wr_stdout (sale s);

// retorna o numero total de vendas contidas no ficheiro
uint64_t no_of_sales (int fd_sale);

// junta o preço e stock de duas vendas
void sale_merge (sale s1, const sale s2);

#endif
