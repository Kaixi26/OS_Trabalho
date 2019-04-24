#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "debug.h"

typedef struct fifo* fifo;

fifo fifo_open_rd       (char* path);
fifo fifo_open_wr       (char* path);

ssize_t fifo_read        (fifo ff, void* buf, size_t count);
ssize_t fifo_write       (fifo ff, void* buf, size_t count);
ssize_t fifo_read_block  (fifo ff, void* buf, size_t count);
ssize_t fifo_write_block (fifo ff, void* buf, size_t count);

void fifo_free (fifo ff);
