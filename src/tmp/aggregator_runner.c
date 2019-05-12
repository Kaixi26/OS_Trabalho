#include "aggregator_runner.h"

#define MIN_SIZE 2048

void aggregator_run (){
    execl("./ag", "./ag", NULL);
}

int aggregator_run_part (int sfd, size_t lo, size_t hi){
    int pp[2];
    pipe (pp);
    if (!fork()){
        dup2 (pp[0], STDIN_FILENO);
        dup2 (pp[1], STDOUT_FILENO);
        aggregator_run ();
        exit (0);
    }
    if (!fork()){
        lseek (sfd, sale_offset (lo), SEEK_SET);
        for (size_t i = lo; i < hi; i++)
            sale_write (pp[0], sale_read_next (sfd));
        exit (0);
    }
    return pp[1];
}

int aggregator_run_aux (int sfd, size_t lo, size_t hi){
    if (hi - lo < MIN_SIZE)
        return aggregator_run_part (sfd, lo, hi);

    return 0;
}

void aggregator_run_conc (){

}
