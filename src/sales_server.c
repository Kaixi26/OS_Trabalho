#if SERVER

#include "sales_server.h"
#include <stdio.h>

static cli_id_type global_id_counter = 1;

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

/* TODO: ERROR CHECKING */
int req_handle_connect (fifo ff_out){
    char temp[1024];
    snprintf (temp, 1024, "%s%d%s", CLIENT_PRE_PATH, global_id_counter, PIPES_EXTENSION);
    mkfifo (temp, 0666);
    fifo_write (ff_out, &global_id_counter, sizeof (cli_id_type));
    global_id_counter++;
    return 0;
}

/* TODO: NEEDS CACHE STRUCT */
int req_handle_show (request req){
    return 0;
}

int req_handle_close (request req){
    char temp[1024];
    cli_id_type cid = req_cli_id (req);
    snprintf (temp, 1024, "%s%d%s", CLIENT_PRE_PATH, cid, PIPES_EXTENSION);
    unlink (temp);
    return 0;
}

int req_handler (request req, fifo ff_out){
    switch (req_type (req)){
    case reqt_connect:
        req_handle_connect (ff_out);
        break;
    case reqt_close:
        req_handle_close (req);
        break;
    default:
        break;
    }
    return 0;
}

int main (){
    int item_fd = open_item_fd ();
    int stock_fd = open_stock_fd ();
    int sales_fd = open_sales_fd();
    PIPES_MKDIR ();
    mkfifo (SERVER_IN_PATH, 0666);
    mkfifo (SERVER_OUT_PATH, 0666);
    fifo ff_in  = fifo_open_rd (SERVER_IN_PATH);
    fifo ff_out = fifo_open_wr (SERVER_OUT_PATH);
    fifo ff_out_locker = fifo_open_rd (SERVER_OUT_PATH);

    while (1){
        request req = req_from_pipe_block (ff_in);
        if (req == NULL)
            continue;
        req_handler (req, ff_out);
        req_free (req);
    }
    /*
    stock_add (1, 10, stock_fd);
    sale tmp = sale_creat (1, 3, 1);
    sale_stock_update (tmp, stock_fd, sales_fd);
    */
    fifo_free (ff_out_locker);
    //fifo_free (ff_in_locker);
    close (item_fd);
    close (stock_fd);
    close (sales_fd);
    return 0;
}

#endif
