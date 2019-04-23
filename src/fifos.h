#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "debug.h"

typedef struct pipe* fifo;

fifo fifo_open_rd(char* path);
fifo fifo_open_wr_block (char* path);
ssize_t fifo_read_block (fifo ff, void* buf, size_t count);
ssize_t fifo_read (fifo ff, void* buf, size_t count);
ssize_t fifo_write (fifo ff, void* buf, size_t count);
void fifo_free (fifo ff);
