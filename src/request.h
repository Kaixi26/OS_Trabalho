#include "item.h"
#include "sale.h"
#include "fifos.h"
#include "debug.h"

// estrutura que armazena um request
typedef struct request* request;

// tipos possiveis de requests
typedef enum {
      reqt_show
    , reqt_sale
    , reqt_stock
    , reqt_update_cache
    , reqt_reload_cache
    , reqt_aggregate
} req_t;

// retorna o tipo de request
req_t   req_type   (request);
// retorna o id correspondente ao request
iid_t   req_id     (request);
// retorna a quantidade de stock acossiada ao request
stock_t req_amount (request);
// retorna o pid do processo que iniciou o request
pid_t   req_pid    (request);

// aloca espaço para um request
request req_creat (req_t rt, pid_t cli_pid, iid_t id, stock_t amount);
// liberta o espaço reservado para o request
void    req_free (request req);

// escreve um request num fifo, bloqueando
ssize_t req_to_pipe_block   (fifo, request);
// lê um request de um fifo, bloqueando
request req_from_pipe_block (fifo ff);
