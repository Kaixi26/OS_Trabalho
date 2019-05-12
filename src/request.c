#include "request.h"

typedef struct request {
    req_t rt;
    pid_t cli_pid;
    iid_t id;
    stock_t amount;
}* request;

req_t req_type (request req){
    return req->rt;
}

iid_t req_id (request req){
    return req->id;
}

stock_t req_amount (request req){
    return req->amount;
}

pid_t req_pid (request req){
    return req->cli_pid;
}

request req_creat (req_t rt, pid_t cli_pid, iid_t id, stock_t amount){
    request tmp = calloc(1, sizeof(struct request));
    tmp->rt = rt;
    tmp->cli_pid = cli_pid;
    tmp->id = id;
    tmp->amount = amount;
    return tmp;
}

void req_free (request req){
    if(req)
        free(req);
}

ssize_t req_to_pipe_block (fifo ff, request req){
    return fifo_write_block (ff, req, sizeof(struct request));
}

request req_from_pipe_block (fifo ff){
    request tmp = calloc (1, sizeof(struct request));
    if (fifo_read_block (ff, tmp, sizeof(struct request)) != sizeof(struct request)){
        free (tmp);
        return NULL;
    }
    return tmp;
}

/*
#define SIZEOF_REQUEST (int)sizeof(struct request)

typedef struct request {
    request_type rt;
    id_type id;
    cli_id_type cli_id;
    item_amnt_type amount;
}* request;

request req_creat (request_type rt, cli_id_type cli_id){
    request tmp;
    if ((tmp = calloc (1, SIZEOF_REQUEST)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to allocate request.\n", alloc_err);
    tmp->rt = rt;
    tmp->cli_id = cli_id;
    return tmp;
 alloc_err:
    return NULL;
}

request req_creat_connect (void){
    return req_creat (reqt_connect, 0);
}

request req_creat_ (request_type rt, id_type id, cli_id_type cli_id){
    request tmp;
    if ((tmp = req_creat (rt, cli_id)) == NULL)
        return NULL;
    tmp->id = id;
    return tmp;
}

request req_creat__ (request_type rt, id_type id
                     , item_amnt_type amount, cli_id_type cli_id){
    request tmp;
    if ((tmp = req_creat_ (rt, id, cli_id)) == NULL)
        return NULL;
    tmp->amount = amount;
    return tmp;
}

request_type req_type (request req){
    return req->rt;
}

id_type req_id (request req){
    return req->id;
}

stock_am_type req_amount (request req){
    return req->amount;
}

cli_id_type req_cli_id (request req){
    return req->cli_id;
}
void req_free (request req){
    if (req)
        free (req);
}

request req_from_pipe_block (fifo ff){
    request tmp;
    if ((tmp = calloc (1, sizeof(struct request))) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to allocate request.\n", alloc_err);
    if (fifo_read_block (ff, tmp, SIZEOF_REQUEST) != SIZEOF_REQUEST)
        REP_ERR_GOTO_V2 ("Error reading request from pipe.\n", read_err);
    return tmp;
 read_err:
    free (tmp);
 alloc_err:
    return NULL;
}

ssize_t req_to_pipe_block (fifo ff, request req){
    return fifo_write_block (ff, req, SIZEOF_REQUEST);
}

ssize_t req_to_pipe (fifo ff, request req){
    return fifo_write (ff, req, SIZEOF_REQUEST);
}
*/
