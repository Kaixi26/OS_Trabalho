#ifndef __FILES_H__
#define __FILES_H__

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define ITEM_FILE_PATH      "ARTIGOS"
#define ITEM_NAME_FILE_PATH "STRINGS"
#define SALES_FILE_PATH     "VENDAS"
#define STOCK_FILE_PATH     "STOCKS"

#define PIPES_DIR_PATH      "PIPES/"
#define SERVER_IN_PATH      PIPES_DIR_PATH "SERVERIN"
#define SERVER_OUT_PATH     PIPES_DIR_PATH "SERVEROUT"

#define PIPES_MKDIR mkdir (PIPES_DIR_PATH, 0666)

#define FILE_OPEN(path, flags) open (path, flags, 0666)
#define FILE_EXISTS(path) !access (path, F_OK)

#define ITEM_FILE_OPEN(xflags) FILE_OPEN (ITEM_FILE_PATH, xflags)
#define ITEM_NAME_FILE_OPEN(xflags) FILE_OPEN (ITEM_NAME_FILE_PATH, xflags)
#define SALES_FILE_OPEN(xflags) FILE_OPEN (SALES_FILE_PATH, xflags)
#define STOCK_FILE_OPEN(xflags) FILE_OPEN (STOCK_FILE_PATH, xflags)

#define FD_SAVE_POSITION(fd)                                    \
    off_t (__fd_saved_position = lseek (fd, 0, SEEK_CUR))
#define FD_COPY_SAVED_POSITION(fd)              \
    (lseek (fd, __fd_saved_position, SEEK_SET))

#endif
