#include "sales_client.h"

#if _COMPILE_CLIENT
#include <stdio.h>

static struct {
    cli_id_type id;
    fifo ff_out;
    fifo ff_in;
} CLIENT;

static int connect (){
    request req;
    fifo ff_server_out;
    char* path;
    if ((CLIENT.ff_out = fifo_open_wr (SERVER_IN_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to open server in.\n", fifo_open_out_err);
    if ((ff_server_out = fifo_open_rd (SERVER_OUT_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to open server out.\n", fifo_open_err);
    if ((req = req_creat_connect ()) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to create request.\n", req_creat_err);
    if (req_to_pipe_block (CLIENT.ff_out, req) == -1)
        REP_ERR_GOTO_V2 ("Error trying to make request.\n", req_write_err);
    if ((fifo_read_block (ff_server_out, &(CLIENT.id), sizeof(cli_id_type))) == -1)
        REP_ERR_GOTO_V2 ("Error trying to read fifo.\n", fifo_read_err);
    if ((path = files_client_path (CLIENT.id)) == NULL)
        REP_ERR_GOTO_V2 ("Error allocating path.", path_alloc_err);
    if ((CLIENT.ff_in = fifo_open_wr (path)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to read fifo.\n", fifo_read_err);
    req_free (req);
    free (path);
    fifo_free (ff_server_out);
    return 0;
path_alloc_err:
fifo_read_err:
    fifo_free (ff_server_out);
req_write_err:
    req_free (req);
req_creat_err:
fifo_open_err:
    fifo_free (CLIENT.ff_out);
fifo_open_out_err:
    return -1;
}

static int request_show (id_type id){
    request req;
    cache_unit c_unit;
    if ((req = req_creat_ (reqt_show, id, CLIENT.id)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to create request.\n", req_creat_err);
    if (req_to_pipe_block (CLIENT.ff_out, req) == -1)
        REP_ERR_GOTO_V2 ("Failed to make disconnect request.\n", ff_write_err);
    if (fifo_read_block (CLIENT.ff_in, &c_unit, sizeof(cache_unit)) == -1)
        REP_ERR_GOTO_V2 ("Failed to make disconnect request.\n", ff_write_err);
    printf ("%ld %ld\n", c_unit.stock, c_unit.price);
    return 0;
 ff_write_err:
 req_creat_err:
    return -1;
}

static int request_sale (id_type id, stock_am_type st){
    request req;
    if ((req = req_creat__ (reqt_sale, id, st, CLIENT.id)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to create request.\n", req_creat_err);
    if (req_to_pipe_block (CLIENT.ff_out, req) == -1)
        REP_ERR_GOTO_V2 ("Failed to make disconnect request.\n", ff_write_err);
    req_free (req);
    return 0;
 ff_write_err:
    req_free (req);
 req_creat_err:
    return -1;
}

static void disconnect (int signum){
    request req;
    if ((req = req_creat (reqt_close, CLIENT.id)) == NULL)
        return;
    req_to_pipe_block (CLIENT.ff_out, req);
    req_free (req);
    fifo_free (CLIENT.ff_in);
    fifo_free (CLIENT.ff_out);
    exit (signum);
}


int main (){
    signal (SIGINT, disconnect);
    connect ();
    while (1){
        sleep (1);
        request_show (1);
        printf ("cid:%d\n", CLIENT.id);
    }
    return 0;
}
#endif
