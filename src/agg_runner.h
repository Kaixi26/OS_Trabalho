#ifndef __AGGREGATOR_RUNNER__
#define __AGGREGATOR_RUNNER__

#include "sale.h"
#include <sys/wait.h>

// corre o agregador lendo de 'fd_sale' e escrevendo para 'fd_out'
void run_agg (int fd_out, int fd_sale, uint64_t sale_amount);

#endif
