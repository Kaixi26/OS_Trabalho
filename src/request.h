#include "item.h"
#include "sale.h"
#include "fifos.h"
#include "debug.h"

typedef struct request* request;
typedef int32_t cli_id_type;

typedef enum {
    reqt_err
    , reqt_show
    , reqt_sale
    , reqt_update_cache
    , reqt_connect
    , reqt_close
} request_type;

typedef struct {
    cli_id_type id;
    pid_t spid;
} cli_conn_t;

request req_creat_connect (void);
request req_creat   (request_type , cli_id_type);
request req_creat_  (request_type , id_type , cli_id_type);
request req_creat__ (request_type , id_type , item_amnt_type , cli_id_type);
void    req_free    (request);

request_type req_type   (request);
id_type      req_id     (request);
stock_am_type req_amount (request);
cli_id_type  req_cli_id (request);

request req_from_pipe_block (fifo);
ssize_t req_to_pipe_block   (fifo, request);
