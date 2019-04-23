#include "sales_client.h"

#if _COMPILE_CLIENT
int main (){
    fifo ff = fifo_open_wr_block (SERVER_IN_PATH);
    request req = req_creat__ (req_show, 1, 39);
    req_to_pipe_block (ff, req);
    req_free (req);
    fifo_free (ff);
}
#endif
