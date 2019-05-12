#include "item.h"
#include "sale.h"
#include "fifos.h"
#include "debug.h"

typedef struct request* request;

typedef enum {
    reqt_NULL
    , reqt_show
    , reqt_sale
    , reqt_stock
    , reqt_update_cache
    , reqt_aggregate
} req_t;

req_t   req_type   (request);
iid_t   req_id     (request);
stock_t req_amount (request);
pid_t   req_pid    (request);

request req_creat (req_t rt, pid_t cli_pid, iid_t id, stock_t amount);
void    req_free (request req);

ssize_t req_to_pipe_block   (fifo, request);
request req_from_pipe_block (fifo ff);

request req_rd (char* path);
size_t req_wr (char* path, request req);
