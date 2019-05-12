#if _COMPILE_CLIENT

#include "fifos.h"
#include "files.h"
#include "request.h"
#include "arguments.h"
#include "cache.h"
#include "item.h"
#include "sale.h"
#include "debug.h"
#include "signal.h"
#include <sys/types.h>
#include <sys/wait.h>

#define PING_TIMER 5

static struct {
    fifo ff_out;
    fifo ff_in;
    char* in_path;
    pid_t pid;
} CLIENT;

static int connect (){
    CLIENT.in_path =  files_client_path (CLIENT.pid = getpid());
    CLIENT.ff_out = fifo_open_wr (SRV_IN_PATH);
    mkfifo(CLIENT.in_path, 0666);
    CLIENT.ff_in = fifo_open_rd (CLIENT.in_path);
    if (!CLIENT.ff_out || !CLIENT.ff_in)
        exit(0);
    return 0;
}

static void disconnect (){
    unlink(CLIENT.in_path);
    exit(0);
}

static void wait_cunit (int show_price){
    cache_unit cunit;
    fifo_read_block(CLIENT.ff_in, &cunit, sizeof(cunit));
    if (cunit.id != -1){
        if (!show_price) printf ("%ld\n", cunit.stock);
        else printf ("%ld %.2f\n", cunit.stock, cunit.price);
    }
}

static void argparse_show (arguments a){
    iid_t id = atol(arg_argv(a,0));
    if (id < 1) return;
    request req = req_creat(reqt_show, CLIENT.pid, id, 0);
    req_to_pipe_block (CLIENT.ff_out, req);
    wait_cunit(1);
}

static void argparse_stock (arguments a){
    iid_t id    = atol(arg_argv(a,0));
    stock_t stk = atol(arg_argv(a,1));
    if (id < 1 || stk == 0) return;
    request req = req_creat(stk>0 ? reqt_stock : reqt_sale, CLIENT.pid, id, stk);
    req_to_pipe_block (CLIENT.ff_out, req);
    free (req);
    wait_cunit(0);
}

static void argparse (arguments a){
    switch (arg_argc(a)){
    case 1:
        argparse_show(a);
        break;
    case 2:
        argparse_stock(a);
        break;
    default:
        return;
    }
}

static void c_run(){
    signal(SIGQUIT, disconnect);
    signal(SIGTERM, disconnect);
    signal(SIGINT , disconnect);
    signal(SIGUSR1, disconnect);
    char buf[1024];
    while (fgets(buf, 1023, stdin)){
        arguments a = arg_get(buf);
        argparse(a);
        arg_free(a);
    }
    exit(0);
}

static void p_run(pid_t cpid){
    close(STDIN_FILENO);
    signal(SIGCHLD, exit);
    while(1){
        if (!FILE_EXISTS(SRV_IN_PATH)){
            kill(cpid, SIGUSR1);
            wait(NULL);
        }
        sleep(PING_TIMER);
    }
    exit(0);
}

int main (){
    connect();
    pid_t cpid = fork();
    switch(cpid){
    case -1:
        disconnect();
        break;
    case 0:
        c_run();
        break;
    default:
        p_run(cpid);
        break;
    }
}

#endif
