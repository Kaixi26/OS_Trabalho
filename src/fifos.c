#include "fifos.h"

typedef enum {
    fifo_rd
    , fifo_wr
} fifo_type;

typedef struct fifo {
    int fd;
    char* path;
    fifo_type type;
}* fifo;

fifo fifo_open (char* path, int flags){
    fifo p;
    if ((p = calloc (1, sizeof (struct fifo))) == NULL)
        REP_ERR_GOTO_V2 ("Error allocating fifo.\n", alloc_err);
    if (!path || !(p->path = strdup (path)))
        REP_ERR_GOTO_V2 ("Error to duplicate fifo path.\n", path_dup_err);
    if ((p->fd = open (p->path, flags)) == -1)
        REP_ERR_GOTO_V2 ("Error trying to open fifo.\n", open_err);
    return p;
open_err:
    free (p->path);
path_dup_err:
    free (p);
alloc_err:
    return NULL;
}

int fifo_reopen (fifo ff){
    close (ff->fd);
    if (ff->type == fifo_rd)
        ff->fd = open (ff->path, O_RDONLY | O_NONBLOCK);
    else if (ff->type == fifo_wr)
        ff->fd = open (ff->path, O_WRONLY | O_NONBLOCK);
    return ff->fd;
}

int fifo_reopen_block (fifo ff){
    close (ff->fd);
    if (ff->type == fifo_rd)
        ff->fd = open (ff->path, O_RDONLY);
    else if (ff->type == fifo_wr)
        ff->fd = open (ff->path, O_WRONLY);
    return ff->fd;
}

fifo fifo_open_rd (char* path){
    fifo p;
    if ((p = fifo_open (path, O_RDONLY | O_NONBLOCK)) == NULL)
        return NULL;
    p->type = fifo_rd;
    return p;
}

fifo fifo_open_wr_block (char* path){
    fifo p;
    if ((p = fifo_open (path, O_WRONLY)) == NULL)
        return NULL;
    p->type = fifo_wr;
    return p;
}

fifo fifo_open_wr (char* path){
    fifo p;
    if ((p = calloc (1, sizeof (struct fifo))) == NULL)
        REP_ERR_GOTO_V2 ("Error allocating fifo.\n", alloc_err);
    if ((p->path = strdup (path)) == NULL)
        REP_ERR_GOTO_V2 ("Error duplicating path.\n", strdup_err);
    p->fd = open (p->path, O_WRONLY | O_NONBLOCK);
    p->type = fifo_wr;
    return p;
strdup_err:
    free (p);
alloc_err:
    return NULL;
}

ssize_t fifo_read_block (fifo ff, void* buf, size_t count){
    if (count <= 0)
        return -1;
    ssize_t rd = 0;
    if ((rd = read (ff->fd, buf, count)) <= 0){
        fifo_reopen_block (ff);
        rd = read (ff->fd, buf, count);
    }
    return rd;
}

ssize_t fifo_write_block (fifo ff, void* buf, size_t count){
    if (count <= 0)
        return -1;
    ssize_t wr = 0;
    if ((wr = write (ff->fd, buf, count)) <= 0){
        fifo_reopen_block (ff);
        wr = write (ff->fd, buf, count);
    }
    return wr;
}

ssize_t fifo_read (fifo ff, void* buf, size_t count){
    fifo_reopen (ff);
    ssize_t rd;
    rd = read (ff->fd, buf, count);
    return rd;
}

ssize_t fifo_write (fifo ff, void* buf, size_t count){
    fifo_reopen (ff);
    return write (ff->fd, buf, count);
}

void fifo_free (fifo ff){
    if (ff){
        close ((ff)->fd);
        free ((ff)->path);
        free (ff);
    }
}
