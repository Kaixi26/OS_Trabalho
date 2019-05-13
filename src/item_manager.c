#ifdef _COMPILE_IMAN

#include "request.h"
#include "files.h"
#include "arguments.h"

static struct {
    int fd_item;
    int fd_item_n;
} IMAN;

static int open_fd_item_n (char* path){
    int ret;
    if (FILE_EXISTS(path))
        ret = IMAN.fd_item_n = open(path, O_RDWR | O_CREAT, 0666);
    else {
        ret = open(path, O_RDWR | O_CREAT, 0666);
        n_len_t tmp = 0;
        pwrite(ret, &tmp, sizeof(tmp), 0);
    }
    return ret;
}

static void start (){
    IMAN.fd_item = open(ITEM_FP, O_RDWR | O_CREAT, 0666);
    IMAN.fd_item_n = open_fd_item_n(INAME_FP);
}

static void compact_names(){
    unlink(ITEM_FP  ".tmp");
    unlink(INAME_FP ".tmp");
    int fd_item_tmp   = open(ITEM_FP ".tmp", O_RDWR | O_CREAT, 0666);
    int fd_item_n_tmp = open_fd_item_n(INAME_FP ".tmp");
    iid_t max = item_id_top(IMAN.fd_item);
    item it;
    for (iid_t i = 1; i < max; i++){
        it = item_rd(i, IMAN.fd_item, IMAN.fd_item_n);
        item_wr_new(item_price(it), item_name(it), fd_item_tmp, fd_item_n_tmp);
        free(it);
    }
    unlink(ITEM_FP);
    unlink(INAME_FP);
    link(ITEM_FP  ".tmp", ITEM_FP);
    link(INAME_FP ".tmp", INAME_FP);
    unlink(ITEM_FP  ".tmp");
    unlink(INAME_FP ".tmp");
    request req   = req_creat(reqt_reload_cache, getpid(), 0, 0);
    fifo tmp_fifo = fifo_open_wr(SRV_IN_PATH);
    if (tmp_fifo) req_to_pipe_block (tmp_fifo, req);
    fifo_free(tmp_fifo);
}

static void command_wr (arguments a){
    const char* name = arg_argv(a,1);
    price_t price = atof(arg_argv(a,2));
    if (price > 0)
        item_wr_new(price, name, IMAN.fd_item, IMAN.fd_item_n);
    printf("%ld\n", item_id_top(IMAN.fd_item)-1);
}

static void command_ch_name (arguments a){
    iid_t id = atol(arg_argv(a,1));
    const char* name = arg_argv(a,2);
    if (id > 0)
        item_ch_name(id, name, IMAN.fd_item, IMAN.fd_item_n);
}

static void price_req_send (iid_t id){
    if(!fork()){
        request req   = req_creat(reqt_update_cache, getpid(), id, 0);
        fifo tmp_fifo = fifo_open_wr(SRV_IN_PATH);
        if (tmp_fifo)
            req_to_pipe_block (tmp_fifo, req);
        exit(0);
    }
}
static void aggregate_req_send (){
    if(!fork()){
        request req   = req_creat(reqt_aggregate, getpid(), 0, 0);
        fifo tmp_fifo = fifo_open_wr(SRV_IN_PATH);
        if (tmp_fifo)
            req_to_pipe_block (tmp_fifo, req);
        exit(0);
    }
}

static void command_ch_price (arguments a){
    iid_t id = atol(arg_argv(a,1));
    price_t price = atof(arg_argv(a,2));
    if (id > 0 && price > 0){
        item_ch_price(id, price, IMAN.fd_item);
        price_req_send (id);
    }
}

static void parse_args (arguments a){
    if (!(arg_argc(a) == 1 || arg_argc(a) == 3))
        return;
    if (!strcmp(arg_argv(a,0), "a"))
        aggregate_req_send ();
    else if (!strcmp(arg_argv(a,0), "i"))
        command_wr(a);
    else if (!strcmp(arg_argv(a,0), "n"))
        command_ch_name(a);
    else if (!strcmp(arg_argv(a,0), "p"))
        command_ch_price(a);
}

int main (){
    start();
    char buf[1024];
    while(fgets(buf, 1023, stdin)){
        arguments a = arg_get (buf);
        parse_args(a);
        arg_free (a);
    }
    n_len_t wlen = item_wasted_len (IMAN.fd_item_n);
    n_len_t mlen = item_total_len  (IMAN.fd_item_n)/5;
    if (wlen >= mlen)
        compact_names();
    return 0;
}

#endif
