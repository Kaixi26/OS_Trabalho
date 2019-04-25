#include "sales_client.h"

#if _COMPILE_CLIENT
#include <stdio.h>

#define SERVER_PING_TIMER 5

static struct {
    cli_id_type id;
    fifo ff_out;
    fifo ff_in;
    pid_t cpid;
    pid_t spid;
} CLIENT;

static void sig_ignore (int signum){
    return;
    signum = signum;
}

static int connect (){
    request req;
    fifo ff_server_out;
    char* path;
    cli_conn_t cconn;
    if ((CLIENT.ff_out = fifo_open_wr (SERVER_IN_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to open server in.\n", fifo_open_out_err);
    if ((ff_server_out = fifo_open_rd (SERVER_OUT_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to open server out.\n", fifo_open_err);
    if ((req = req_creat_connect ()) == NULL)
        REP_ERR_GOTO_V2 ("Error trying to create request.\n", req_creat_err);
    if (req_to_pipe_block (CLIENT.ff_out, req) == -1)
        REP_ERR_GOTO_V2 ("Error trying to make request.\n", req_write_err);
    if ((fifo_read_block (ff_server_out, &(cconn), sizeof(cli_conn_t))) == -1)
        REP_ERR_GOTO_V2 ("Error trying to read fifo.\n", fifo_read_err);
    CLIENT.id = cconn.id;
    CLIENT.spid = cconn.spid;
    if ((path = files_client_path (CLIENT.id)) == NULL)
        REP_ERR_GOTO_V2 ("Error allocating path.", path_alloc_err);
    if ((CLIENT.ff_in = fifo_open_rd (path)) == NULL)
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
    printf ("%ld %f\n", c_unit.stock, item_price_to_double (c_unit.price));
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

static int argparse (arguments a){
    int ret;
    id_type id;
    stock_am_type stock;
    switch (arg_argc (a)){
    case 1:
        if ((id = atol (arg_argv (a, 0))) <= 0)
            REP_ERR_GOTO_V2 ("Invalid item id.\n", bad_argv_err);
        ret = request_show (id);
        break;
    case 2:
        if ((id = atol (arg_argv (a, 0))) <= 0)
            REP_ERR_GOTO_V2 ("Invalid item id.\n", bad_argv_err);
        if ((stock = atol (arg_argv (a, 1))) == 0)
            REP_ERR_GOTO_V2 ("Invalid stock change.\n", bad_argv_err);
        ret = request_sale (id, stock);
        break;
    default:
        REP_ERR_GOTO_V2 ("Wrong amount of arguments.\n", bad_argc_err);
        break;
    }
    return ret;
bad_argv_err:
bad_argc_err:
    return -2;
}

static void child_disconnect (int signum){
    if (signum == SIGINT)
        printf ("Sigint\n");
    request req;
    if ((req = req_creat (reqt_close, CLIENT.id)) == NULL)
        exit (signum);
    req_to_pipe_block (CLIENT.ff_out, req);
    req_free (req);
    fifo_free (CLIENT.ff_in);
    fifo_free (CLIENT.ff_out);
    printf ("Server connection to closed.\n");
    exit (signum);
}

static void parent_disconnect (int signum){
    kill (CLIENT.cpid, SIGUSR1);
    wait (NULL);
    exit (signum);
}


int main (){
    int err;
    connect ();
    switch ((CLIENT.cpid = fork())){
    case -1:
        break;
    case 0:
        signal (SIGTERM, sig_ignore);
        signal (SIGINT,  sig_ignore);
        signal (SIGKILL, child_disconnect);
        signal (SIGUSR1, child_disconnect);
        char buf[1024];
        do {
            if (fgets (buf, 1024, stdin) == NULL)
                break;
            arguments a = arg_get (buf);
            err = argparse (a);
            arg_dest (&a);
        } while (err != -1);
        child_disconnect (1);
        exit (1);
        break;
    default:
        close (STDIN_FILENO);
        signal (SIGTERM, parent_disconnect);
        signal (SIGINT, parent_disconnect);
        signal (SIGQUIT, parent_disconnect);
        signal (SIGKILL, parent_disconnect);
        signal (SIGCHLD, exit);
        while (kill (CLIENT.spid, 0) != -1){
            sleep (SERVER_PING_TIMER);
        }
        printf ("Connection to server lost.\n");
        kill (CLIENT.cpid, SIGUSR1);
        break;
    }
    return 0;
}
#endif
