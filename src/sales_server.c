#if _COMPILE_SERVER

#include "sales_server.h"
#include <stdio.h>

#define SERVER_TIMEOUT 10

static struct {
    int stock_fd;
    int sales_fd;
    fifo ff_in;
    fifo ff_out;
    cache cache;
    cli_id_type cid_curr;
} SERVER;


static int open_item_fd (){
    int fd;
    if ((fd = ITEM_FILE_OPEN(O_RDONLY) == -1))
        REP_ERR_GOTO_V2 ("Error opening items file.\n", open_err);
    return fd;
open_err:
    return -1;
}

static int open_stock_fd (){
    int fd;
    if ((fd = STOCK_FILE_OPEN(O_RDWR | O_CREAT)) == -1)
        REP_ERR_GOTO_V2 ("Error open stock file.\n", open_err);
    return fd;
 open_err:
    return -1;
}

static int open_sales_fd (){
    int fd;
    if ((fd = SALES_FILE_OPEN(O_RDWR | O_CREAT)) == -1)
        REP_ERR_GOTO_V2 ("Error open sales file.\n", open_err);
    return fd;
 open_err:
    return -1;
}


/* TODO: WAY TO REPORT TO PARENT */
static void connect_timeout (int signum){
    printf ("Timed out.\n");
    exit (signum);
}

static void show_timeout (int signum){
    printf ("Show timed out.\n");
    exit (signum);
}

static int req_handle_connect_forked (cli_id_type cid){
    SERVER.cid_curr = cid;
    signal (SIGALRM, connect_timeout);
    char *path = files_client_path (cid);
    mkfifo (path, 0666);
    alarm (SERVER_TIMEOUT);
    fifo_write_block (SERVER.ff_out, &SERVER.cid_curr, sizeof (cli_id_type));
    alarm (0);
    printf ("[Connected]:\t%s.\n", path);
    exit (0);
}

static int req_handle_show_forked (request req){
    signal (SIGALRM, connect_timeout);
    id_type id = req_id (req);
    cli_id_type cid = req_id (req);
    char *path = files_client_path (cid);
    cache_unit c_unit = cache_get (SERVER.cache, id);
    fifo out = fifo_open_wr (path);
    alarm (SERVER_TIMEOUT);
    fifo_write_block (out, &c_unit, sizeof (cache_unit));
    alarm (0);
    exit (0);
}

/* TODO: ERROR CHECKING */
int req_handle_connect (){
    if (!fork())
        req_handle_connect_forked (SERVER.cid_curr);
    SERVER.cid_curr++;
    return 0;
}

/* TODO: NEEDS CACHE STRUCT */
int req_handle_show (request req){
    if (!fork ())
        req_handle_show_forked (req);
    return 0;
}

int req_handle_close (request req){
    cli_id_type cid = req_cli_id (req);
    char* path = files_client_path (cid);
    unlink (path);
    printf ("[Disconnected]:\t%s.\n", path);
    free (path);
    return 0;
}

int req_handler (request req){
    switch (req_type (req)){
    case reqt_connect:
        req_handle_connect ();
        break;
    case reqt_close:
        req_handle_close (req);
        break;
    default:
        break;
    }
    return 0;
}

static int boot (){
    if ((SERVER.sales_fd = open_sales_fd ()) == -1)
        goto open_sales_fd_err;
    if ((SERVER.stock_fd = open_stock_fd ()) == -1)
        goto open_stock_fd_err;
    PIPES_MKDIR ();
    mkfifo (SERVER_IN_PATH, 0666);
    mkfifo (SERVER_OUT_PATH, 0666);
    if ((SERVER.ff_in  = fifo_open_rd (SERVER_IN_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error opening server in fifo.\n", fifo_in_err);
    if ((SERVER.ff_out = fifo_open_wr (SERVER_OUT_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error opening server out fifo.\n", fifo_out_err);
    if ((SERVER.cache = cache_creat (STOCK_FILE_PATH, ITEM_FILE_PATH)) == NULL)
        REP_ERR_GOTO_V2 ("Error allocating the cache.\n", cache_creat_err);
    SERVER.cid_curr = 1;
    return 0;
cache_creat_err:
fifo_out_err:
    fifo_free (SERVER.ff_in);
fifo_in_err:
    close (SERVER.stock_fd);
open_stock_fd_err:
    close (SERVER.sales_fd);
open_sales_fd_err:
    return -1;
}


int main (){
    if (boot () == -1)
        return -1;

    while (1){
        request req = req_from_pipe_block (SERVER.ff_in);
        if (req == NULL)
            continue;
        req_handler (req);
        req_free (req);
    }
    /*
    stock_add (1, 10, stock_fd);
    sale tmp = sale_creat (1, 3, 1);
    sale_stock_update (tmp, stock_fd, sales_fd);
    */
    return 0;
}

#endif
