/*
#include "item_manager.h"
#define SERVER_TIMEOUT 30


static int create_item_fd (){
    int fd;
    if ((fd = ITEM_FILE_OPEN(O_RDWR | O_CREAT)) == -1)
        REP_ERR_GOTO_V2 ("Error creating item files.\n", creat_failure);
    if (item_fill_id0 (fd) == -1){
        goto write_failure;
    }
    return fd;
 write_failure:
    close (fd);
 creat_failure:
    return -1;
}

static int open_item_fd (){
    int fd;
    if (FILE_EXISTS (ITEM_FILE_PATH)){
        if ((fd = ITEM_FILE_OPEN(O_RDWR)) == -1)
            REP_ERR_GOTO_V2 ("Error opening item file.\n", open_failure);
    }else{
        if ((fd = create_item_fd ()) == -1)
            goto open_failure;
    }
    return fd;
 open_failure:
    return -1;
}

static int create_item_name_fd (){
    int fd;
    uint64_t tmp = 0;
    if ((fd = ITEM_NAME_FILE_OPEN(O_RDWR | O_CREAT)) == -1)
        REP_ERR_GOTO_V2 ("Error creating item name files.\n", creat_failure);
    if ((write (fd, &tmp, sizeof(tmp)))){
        goto write_failure;
    }
    return fd;
 write_failure:
    close (fd);
 creat_failure:
    return -1;
}

static int open_item_name_fd (){
    int fd;
    if (FILE_EXISTS (ITEM_NAME_FILE_PATH)){
        if ((fd = ITEM_NAME_FILE_OPEN(O_RDWR)) == -1)
            REP_ERR_GOTO_V2 ("Error opening item name file.\n", open_failure);
    }
    else
        fd = create_item_name_fd ();
    return fd;
 open_failure:
    return -1;
}

void iman_send_update_request (id_type id){
    if (!fork()){
        fifo ff_srv_in = fifo_open_wr (SERVER_IN_PATH);
        request req = req_creat_ (reqt_update_cache, id, id);
        signal (SIGALRM, exit);
        alarm (SERVER_TIMEOUT);
        req_to_pipe_block   (ff_srv_in, req);
        fifo_free (ff_srv_in);
        exit(0);
    }
}

int iman_add (arguments a, int a_depth, int item_fd, int item_name_fd){
    const char* name;
    const char* price_str;
    double price_double;
    price_type price;
    int id;
    if (!(name = arg_argv (a, a_depth++)))
        REP_ERR_GOTO_V2 ("Could not add item: couldn't get name.\n", invalid_input_failure);
    if (!(price_str = arg_argv (a, a_depth)))
        REP_ERR_GOTO_V2 ("Could not add item: couldn't get price.\n", invalid_input_failure);
    price_double = atof (price_str);
    if (price_double <= 0)
        REP_ERR_GOTO_V2 ("Could not add item: invalid price.\n", invalid_input_failure);
    price = double_to_item_price (price_double);
    if ((id = item_new_append (price, name, item_fd, item_name_fd)) == -1){
        goto append_failure;
    }
    return id;
 append_failure:
 invalid_input_failure:
    return -1;
}

int iman_set_name (arguments a, int a_depth, int item_fd, int item_name_fd){
    id_type id = atol (arg_argv (a, a_depth++));
    if (item_amount (item_fd) < id)
        return -1;
    item it = item_read (id, item_fd, item_name_fd);
    //int len = item_name_len (it);
    const char* name = arg_argv (a, a_depth);
    item_name_set (it, name);
    item_write (it, item_fd, item_name_fd);
    //add_unused_item_name (len, item_name_fd);
    return 0;
}

int iman_set_price (arguments a, int a_depth, int item_fd, int item_name_fd){
    id_type id = atol (arg_argv (a, a_depth++));
    if (item_amount (item_fd) < id)
        return -1;
    item it = item_read (id, item_fd, item_name_fd);
    price_type price;
    double price_double = atof (arg_argv (a, a_depth));
    if (price_double <= 0)
        goto bad_input_failure;
    price = double_to_item_price(price_double);
    item_price_set (it, price);
    item_write (it, item_fd, item_name_fd);
    iman_send_update_request (item_id(it));
    return 0;
 bad_input_failure:
    return -1;
}

int iman_print (arguments a, int a_depth, int item_fd, int item_name_fd){
    item it = item_read (atoi (arg_argv (a, a_depth)), item_fd, item_name_fd);
    if (it){
        printf ("%f %s\n", item_price_to_double (item_price (it)), item_name (it));
    }
    item_free (it);
    return 0;
}

int iman_arg_parse (arguments a, int item_fd, int item_name_fd){
    int r = -1;
    if (STR_EQ (arg_argv (a, 0), "print")){
        r = iman_print (a, 1, item_fd, item_name_fd);
    }else  if (arg_argc (a) < 3)
        REP_ERR_GOTO_V2 ("Not enought arguments to parse.\n", missing_args_failure);
    if (STR_EQ (arg_argv (a, 0), "i")){
        r = iman_add (a, 1, item_fd, item_name_fd);
    }
    if (STR_EQ (arg_argv (a, 0), "n")){
        r = iman_set_name (a, 1, item_fd, item_name_fd);
    }
    if (STR_EQ (arg_argv (a, 0), "p")){
        r = iman_set_price (a, 1, item_fd, item_name_fd);
    }
    if (r == -1){
        goto bad_input_failure;
    }
    return r;
 bad_input_failure:
 missing_args_failure:
    return -1;
}

#if (ITEM_MANAGER)
int main (){
    int item_fd = open_item_fd ();
    int item_name_fd = open_item_name_fd();
    char buffer[10240];
    int id;
    while (fgets (buffer, 10240, stdin)){
        arguments a = arg_get (buffer);
        if ((id =iman_arg_parse (a, item_fd, item_name_fd)) > 0)
            printf ("%d\n", id);
        arg_dest (&a);
    }
    close (item_fd);
    close (item_name_fd);
    return 0;
}
#endif
*/
#ifdef _COMPILE_IMAN

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "request.h"
#include "files.h"
#include "item.h"
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
