#include "sales_server.h"

static int open_item_fd (){
    int fd;
    if ((fd = ITEM_FILE_OPEN(O_RDWR) == -1))
        RET_ERR_GOTO_V2 ("Error opening items file.\n", open_err);
open_err:
    return -1;
}

#if SERVER
int main (){

}
#endif
