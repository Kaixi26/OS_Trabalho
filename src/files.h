#ifndef __FILES_H__
#define __FILES_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ITEM_FP      "ARTIGOS"
#define INAME_FP     "STRINGS"
#define SALES_FP     "VENDAS"
#define STOCK_FP     "STOCKS"

#define PIPES_DP      "PIPES/"
#define PIPES_EXT     ".fifo"
#define SRV_IN_PATH   PIPES_DP "server_in" PIPES_EXT
#define SRV_OUT_PATH  PIPES_DP "server_out" PIPES_EXT
#define CLI_PRE_PATH  PIPES_DP "client_"


#define PIPES_MKDIR() mkdir (PIPES_DP, 0777)
#define PIPES_RMDIR() {                                                 \
        if (!fork())                                                    \
            execlp ("rm", "rm", "-r", "-f", PIPES_DP, NULL);            \
        wait (NULL);                                                    \
}

#define FILE_OPEN(path, flags) open (path, flags, 0666)
#define FILE_EXISTS(path) !access (path, F_OK)

char* files_client_path (pid_t pid);

#endif
