#if SERVER

#include "sales_server.h"

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


int main (){
    int item_fd = open_item_fd ();
    int stock_fd = open_stock_fd ();
    int sales_fd = open_sales_fd();
    PIPES_MKDIR ();
    mkfifo (SERVER_IN_PATH, 0666);
    mkfifo (SERVER_OUT_PATH, 0666);
    fifo out_rd = fifo_open_rd (SERVER_OUT_PATH);
    fifo out_wr = fifo_open_wr_block (SERVER_OUT_PATH);
    fifo_write (out_wr, "asdf", 4);
    char buf[100];
    sleep (5);
    fifo_read_block (out_rd, buf, 4);
    write (STDOUT_FILENO, buf, 4);
    /*
    stock_add (1, 10, stock_fd);
    sale tmp = sale_creat (1, 3, 1);
    sale_stock_update (tmp, stock_fd, sales_fd);
    */
    close (item_fd);
    close (stock_fd);
    close (sales_fd);
    return 0;
}

#endif
