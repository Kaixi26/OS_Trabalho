#include "agg_runner.h"

#define CHLD_PROC 12

/*
  fd[1] -> input  of ./ag
  fd[0] -> output of ./ag
*/
void run_singl (int fd_out, int fd_sale, uint64_t begin, uint64_t size){
    if (size == 0) exit(0);
    int fd[2];
    pipe(fd);
    if (!fork()){
        close (fd[0]);
        close (fd_out);
        uint64_t end = begin + size;
        for (uint64_t i = begin; i < end; i++){
            sale s = sale_rd(i, fd_sale);
            write(fd[1], s, sale_size());
        }
        exit(0);
    }
    close(fd[1]);
    if (!fork()){
        dup2(fd[0], STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        execl("./ag", "./ag", NULL);
        exit(0);
    }
    close(fd[0]);
    exit(0);
}

void run_conc (int fd_out, int fd_sale, uint64_t size){
    uint64_t workload = size / CHLD_PROC;
    uint64_t worklst = workload + (size % CHLD_PROC);
    int fd[2];
    pipe(fd);
    for (int i=0; i<CHLD_PROC-1; i++)
        if (!fork()){
            close(fd[0]);
            run_singl(fd[1], fd_sale, i*workload, workload);
        }
    if (!fork()){
        close(fd[0]);
        run_singl(fd[1], fd_sale, (CHLD_PROC-1)*workload, worklst);
    }
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    dup2(fd_out, STDOUT_FILENO);
    execl("./ag", "./ag", NULL);
    exit(0);
}

void run_agg (int fd_out, int fd_sale, uint64_t sale_amount){
    pid_t cpid;
    if (!(cpid = fork()))
        run_conc(fd_out, fd_sale, sale_amount);
    waitpid(cpid, NULL, 0);
}
