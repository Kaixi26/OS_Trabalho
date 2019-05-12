#if _COMPILE_SERVER
#include <time.h>
#include "agg_runner.h"
#include "item.h"
#include "request.h"
#include "stock.h"
#include "sale.h"
#include "fifos.h"
#include "files.h"
#include "cache.h"

#define SHOW_WAIT_TIME 20

static struct {
    int fd_stock;
    int fd_sales;
    int fd_item;
    fifo ff_in;
    cache cach;
} SERVER;

static char* get_str_time (){
    char tmp[1024];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(tmp, 1023, "%Y-%m-%d_%H:%M:%S", t);
    return strdup (tmp);
}

static void reqh_aggregate (){
    int fd[2];
    char* t_str = get_str_time();
    fd[0] = open(SALES_FP, O_RDONLY);
    fd[1] = open(t_str, O_WRONLY | O_CREAT, 0666);
    free(t_str);
    run_agg(fd[1], fd[0], no_of_sales(fd[0]));
    close(fd[0]);
    close(fd[1]);
    unlink(SALES_FP);
    close(SERVER.fd_sales);
    SERVER.fd_sales = open(SALES_FP, O_RDWR | O_CREAT , 0666);
}

static int boot (){
    PIPES_RMDIR();
    PIPES_MKDIR();
    SERVER.fd_stock = open(STOCK_FP, O_RDWR | O_CREAT, 0666);
    SERVER.fd_sales = open(SALES_FP, O_RDWR | O_CREAT, 0666);
    SERVER.fd_item  = open(ITEM_FP , O_RDWR | O_CREAT, 0666);
    SERVER.cach     = cache_creat(STOCK_FP, ITEM_FP);
    mkfifo(SRV_IN_PATH, 0666);
    SERVER.ff_in = fifo_open_rd (SRV_IN_PATH);
    if (SERVER.fd_stock == -1 || SERVER.fd_sales == -1
        || SERVER.fd_item == -1 || !SERVER.ff_in
        || !SERVER.cach)
        exit(0);
    return 0;
}

static void shutdown (int signum){
    signum=signum;
    kill(SIGQUIT, -1);
    PIPES_RMDIR();
    cache_free(SERVER.cach);
    exit (0);
}

static void reqh_show_err (request req){
    if(!fork()){
        cache_unit tmp = (cache_unit){-1,-1,-1};
        fifo tmp_fifo = fifo_open_wr(files_client_path(req_pid(req)));
        signal(SIGALRM, exit);
        alarm(SHOW_WAIT_TIME);
        fifo_write_block (tmp_fifo, &tmp, sizeof(tmp));
        exit(0);
    }
}

static void reqh_show (request req){
    if(!fork()){
        cache_unit tmp = cache_get(SERVER.cach, req_id(req));
        fifo tmp_fifo = fifo_open_wr(files_client_path(req_pid(req)));
        signal(SIGALRM, exit);
        alarm(SHOW_WAIT_TIME);
        fifo_write_block (tmp_fifo, &tmp, sizeof(tmp));
        exit(0);
    }
}

static void reqh_add_stock (request req){
    iid_t id = req_id(req);
    stock_t amount = req_amount(req);
    cache_inc_stock(SERVER.cach, id, amount) == -1 ?
        reqh_show_err(req) : reqh_show(req);
}

static void reqh_sale (request req){
    iid_t id = req_id(req);
    stock_t amount = req_amount(req);
    if (cache_inc_stock(SERVER.cach, id, amount) == -1)
        reqh_show_err(req);
    else {
        sale s = sale_creat(id, cache_get_price(SERVER.cach, id) * amount, amount);
        sale_wr(s, SERVER.fd_sales);
        sale_free(s);
        reqh_show(req);
    }
}

static void reqh_update_cache (request req){
    cache_reload_price (SERVER.cach, req_id(req));
}

static void reqh (request req){
    switch(req_type(req)){
    case reqt_show:
        reqh_show(req);
        break;
    case reqt_stock:
        reqh_add_stock(req);
        break;
    case reqt_sale:
        reqh_sale(req);
        break;
    case reqt_update_cache:
        reqh_update_cache(req);
        break;
    case reqt_aggregate:
        reqh_aggregate(req);
        break;
    default:
        break;
    }
}

static void clean_child (int signum){
    signum=signum;
    wait(NULL);
}

static void set_signals (){
    signal(SIGINT,shutdown);
    signal(SIGCHLD, clean_child);
}

int main (){
    boot();
    set_signals();
    while (1){
        request req = req_from_pipe_block(SERVER.ff_in);
        if(req) reqh(req);
        req_free(req);
        usleep(1000);
    }
    shutdown(0);
}
#endif
