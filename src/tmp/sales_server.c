#if _COMPILE_SERVER

#include "sales_server.h"
#include "aggregator_runner.h"
#include <stdio.h>

#define SERVER_TIMEOUT 30

static struct {
    int stock_fd;
    int sales_fd;
    fifo ff_in;
    fifo ff_out;
    cache cache;
    request_type parent_req;
    cli_id_type cid_curr;
    int force_boot;
    int cpid;
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
    SERVER.parent_req = reqt_NULL;
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

static void shutdown (int signum){
    printf ("[Shutdown]: Shuting server down.\n");
    PIPES_RMDIR ();
    cache_free (SERVER.cache);
    fifo_free (SERVER.ff_out);
    fifo_free (SERVER.ff_in);
    close (SERVER.sales_fd);
    close (SERVER.stock_fd);
    exit (signum);
}

/* TODO: WAY TO REPORT TO PARENT */
static void connect_timeout (int signum){
    fprintf (stderr, "Timed out.\n");
    exit (signum);
}

static void show_timeout (int signum){
    fprintf (stderr, "Show timed out.\n");
    exit (signum);
}

static int req_handle_aggregate (){
    int fd[2];
    int cpid;
    fd[0] = open (SALES_FILE_PATH, O_RDONLY);
    fd[1] = open ("tmp", O_WRONLY | O_CREAT, 0666);
    if (!(cpid = fork ())){
        dup2 (fd[0], STDIN_FILENO);
        dup2 (fd[1], STDOUT_FILENO);
        aggregator_run ();
        exit (-1);
    }
    close (fd[0]);
    close (fd[1]);
    waitpid (cpid, NULL, WNOHANG);
    return 0;
}

static int req_handle_connect_forked (cli_id_type cid){
    cli_conn_t cconn;
    cconn.id = cid;
    cconn.spid = getppid ();
    signal (SIGALRM, connect_timeout);
    char *path = files_client_path (cid);
    mkfifo (path, 0666);
    alarm (SERVER_TIMEOUT);
    fifo_write_block (SERVER.ff_out, &cconn, sizeof (cli_conn_t));
    alarm (0);
    printf ("[Connected]:\t%s.\n", path);
    exit (0);
}

static int req_handle_show_forked (id_type id, cli_id_type cid){
    char *path = files_client_path (cid);
    cache_unit c_unit = cache_get (SERVER.cache, id);
    fifo out = fifo_open_wr (path);
    alarm (SERVER_TIMEOUT);
    fifo_write_block (out, &c_unit, sizeof (cache_unit));
    //printf ("[Request]:\tShown id %ld to client %d.\n", id, cid);
    alarm (0);
    exit (0);
}

/* TODO: ERROR CHECKING */
static int req_handle_connect (){
    if (!fork())
        req_handle_connect_forked (SERVER.cid_curr);
    SERVER.cid_curr++;
    return 0;
}

/* TODO: NEEDS CACHE STRUCT */
static int req_handle_show (request req){
    if (!fork ())
        req_handle_show_forked (req_id (req), req_cli_id (req));
    return 0;
}

static int req_handle_sale (request req){
    id_type id = req_id (req);
    cli_id_type cid = req_cli_id (req);
    //cli_id_type cid = req_cli_id (req);
    id_type max_id = cache_get_maxid (SERVER.cache);
    stock_am_type stock = req_amount (req);
    if (id > max_id){
        if (!fork ())
            req_handle_show_forked (id, cid);
        return -1;
    }
    if (stock < 0){
        cache_unit c_unit = cache_get (SERVER.cache, id);
        if (c_unit.price == 0)
            goto c_unit_err;
        sale s = sale_creat (id, c_unit.price, -stock);
        if (s == NULL)
            REP_ERR_GOTO_V2 ("Error trying to create sale.\n", sale_err);
        if (sale_write_end (s, SERVER.sales_fd))
        REP_ERR_GOTO_V2 ("Error trying to update sale.\n", sale_err);
        //printf ("[Request]:\tSold %ld from id %ld stock to %d.\n", -stock, id, cid);
    }
    cache_update_stock (SERVER.cache, id, stock);
    if (!fork ())
        req_handle_show_forked (id, cid);
    return 0;
 sale_err:
 c_unit_err:
    return -1;
}

static int req_handle_update_cache (request req){
    printf ("%ld\n", req_id(req));
    cache_update_price (SERVER.cache, req_id(req));
    return 0;
}

static int req_handle_close (request req){
    cli_id_type cid = req_cli_id (req);
    char* path = files_client_path (cid);
    unlink (path);
    printf ("[Disconnected]:\t%s.\n", path);
    free (path);
    return 0;
}

static int req_handler (request req){
    switch (req_type (req)){
    case reqt_connect:
        req_handle_connect ();
        break;
    case reqt_close:
        req_handle_close (req);
        break;
    case reqt_show:
        req_handle_show (req);
        break;
    case reqt_sale:
        req_handle_sale (req);
        break;
    case reqt_quit:
        shutdown (0);
        break;
    case reqt_agg:
        req_handle_aggregate ();
        break;
    case reqt_update_cache:
        req_handle_update_cache (req);
        break;
    default:
        break;
    }
    return 0;
}

int child_run (){
    close (STDIN_FILENO);
    if (boot () == -1)
        exit (-1);
    signal (SIGINT, shutdown);
    signal (SIGKILL, shutdown);
    signal (SIGTERM, shutdown);
    while (1){
        request req = req_from_pipe_block (SERVER.ff_in);
        if (req == NULL)
            continue;
        req_handler (req);
        req_free (req);
    }
    exit (0);
}

int main (int argc, char** argv){
    SERVER.force_boot = 0;
    for (int i=1; i<argc; i++)
        if (!strcmp (argv[i], "-f"))
            SERVER.force_boot = 1;
    if (FILE_EXISTS (PIPES_DIR_PATH)){
        if (SERVER.force_boot){
            PIPES_RMDIR();
        }
        else
            REP_ERR_GOTO_V2 ("Another server might be running or has been closed \
forcefully, to force server boot use '-f' flag, beware this might cause problems \
if other server is running.\n", opened_pipes_err);
    }

    if (!(SERVER.cpid = fork ()))
        child_run ();

    signal (SIGCHLD, exit);

    char buf[1024];
    request req_q = req_creat (reqt_quit, 0);
    request req_agg = req_creat (reqt_agg, 0);
    while (1){
        fgets (buf, 1024, stdin);
        fifo tmp = fifo_open_wr (SERVER_IN_PATH);
        if (!strcmp (buf, "exit\n"))
            req_to_pipe_block (tmp, req_q);
        if (!strcmp (buf, "agg\n"))
            req_to_pipe_block (tmp, req_agg);
        fifo_free (tmp);
    }
 opened_pipes_err:
    return -1;
}

#endif
