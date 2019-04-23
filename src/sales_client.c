#include "sales_client.h"

#if _COMPILE_CLIENT

static cli_id_type connect (fifo ff_out){
    request req;
    cli_id_type cid;
    fifo ff_in;
    if ((ff_in = fifo_open_rd (SERVER_OUT_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to open server out.\n", fifo_open_err);
    if ((req = req_creat_connect ()) == NULL)
        goto req_creat_err;
    if (req_to_pipe_block (ff_out, req) == -1)
        REP_ERR_GOTO_V2 ("Error trying to make request.\n", req_write_err);
    if ((fifo_read_block (ff_in, &cid, sizeof(cli_id_type))) == -1)
        goto fifo_read_err;
    req_free (req);
    fifo_free (ff_in);
    return cid;
fifo_read_err:
    fifo_free (ff_in);
req_write_err:
    req_free (req);
req_creat_err:
fifo_open_err:
    return -1;
}

static int disconnect (fifo ff_out, cli_id_type cid){
    request req;
    if ((req = req_creat (reqt_close, cid)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to create request.\n", req_creat_err);
    if (req_to_pipe_block (ff_out, req) == -1)
        REP_ERR_GOTO_V2 ("Failed to make disconnect request.\n", ff_write_err);
    return 0;
ff_write_err:
req_creat_err:
    return -1;
}

int main (){
    fifo ff_out = fifo_open_wr_block (SERVER_IN_PATH);
    cli_id_type cid = connect (ff_out);

    disconnect (ff_out, cid);
    fifo_free (ff_out);
}
#endif
