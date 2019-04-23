#include "item.h"
#include "fifos.h"
#include "debug.h"

typedef enum {
    req_err
    , req_show
    , req_sale
    , req_update_cache
    , req_new
    , req_del
} request_type;

typedef struct request* request;

request req_creat (request_type rt);
request req_creat_ (request_type rt, id_type id);
request req_creat__ (request_type rt, id_type id, item_amnt_type amount);
request_type req_type (request req);
id_type req_id (request req);
id_type req_amount (request req);
void req_free (request req);
request req_from_pipe_block (fifo ff);
ssize_t req_to_pipe_block (fifo ff, request req);
