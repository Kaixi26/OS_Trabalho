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
