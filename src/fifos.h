#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "debug.h"

// estrutura que armazena o fifo
typedef struct fifo* fifo;

// abre um fifo para leitura (é necessário cria-lo previamente com 'mkfifo')
// não bloqueia
fifo fifo_open_rd       (char* path);
// abre um fifo para escrita (é necessário cria-lo previamente com 'mkfifo')
// não bloqueia
fifo fifo_open_wr       (char* path);
// liberta o espaço e descritores armazenados no fifo
void fifo_free (fifo ff);

// bloqueia até conseguir ler do fifo ou obter um erro
ssize_t fifo_read_block  (fifo ff, void* buf, size_t count);
// bloqueia até conseguir escrever para o fifo ou obter um erro
ssize_t fifo_write_block (fifo ff, void* buf, size_t count);
