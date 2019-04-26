#ifdef _COMPILE_PPRINTER

#include <stdio.h>
#include "pprinter.h"

static void print_1 (){
    int item_fd = ITEM_FILE_OPEN (O_RDONLY);
    if (item_fd == -1)
        REP_ERR_GOTO_V2 ("Error opening file.\n", item_open_err);
    int item_name_fd = ITEM_NAME_FILE_OPEN (O_RDONLY);
    if (item_name_fd == -1)
        REP_ERR_GOTO_V2 ("Error opening file.\n", item_name_open_err);
    int amount = item_amount (item_fd);
    item tmp;
    for (int i=1; i<= amount; i++){
        tmp = item_read (i, item_fd, item_name_fd);
        if (tmp){
            printf ("%d %.2f %s\n"
                    , i
                    , item_price_to_double (item_price (tmp))
                    , item_name (tmp));
        }
        else
            continue;
    }
    close (item_name_fd);
 item_name_open_err:
    close (item_fd);
 item_open_err:
    return;
}

static void print_2 (){
    int item_fd = ITEM_FILE_OPEN (O_RDONLY);
    if (item_fd == -1)
        REP_ERR_GOTO_V2 ("Error opening file.\n", item_open_err);
    int item_name_fd = ITEM_NAME_FILE_OPEN (O_RDONLY);
    if (item_name_fd == -1)
        REP_ERR_GOTO_V2 ("Error opening file.\n", item_name_open_err);
    int stock_fd = STOCK_FILE_OPEN (O_RDONLY);
    if (stock_fd == -1)
        REP_ERR_GOTO_V2 ("Error opening file.\n", stock_open_err);
    int amount = item_amount (item_fd);
    stock_am_type tmp;
    for (int i=1; i<= amount; i++){
        tmp = stock_get (i, stock_fd);
        printf ("%d %ld \n", i, tmp);
    }
    close (stock_fd);
 stock_open_err:
    close (item_name_fd);
 item_name_open_err:
    close (item_fd);
 item_open_err:
    return;
}

static void print_3 (){
    int sale_fd = SALES_FILE_OPEN (O_RDONLY);
    if (sale_fd == -1)
        REP_ERR_GOTO_V2 ("Error opening file.\n", sale_open_err);
    sale s = sale_read (0, sale_fd);
    for (int i=1; s; i++){
        printf ("%d %.2f %d\n"
                , sale_id (s)
                , item_price_to_double (sale_total (s))
                , sale_sold (s));
        s = sale_read (i, sale_fd);
    }
    close (sale_fd);
 sale_open_err:
    return;
}

static void print_4 (){
    sale s;
    while ((s = sale_read_next (STDIN_FILENO))){
        printf ("%d %.2f %d\n"
                , sale_id (s)
                , item_price_to_double (sale_total (s))
                , sale_sold (s));
    }
}

int main (int argc, char** argv){
    if (!strcmp (argv[1], "1"))
        print_1 ();
    if (!strcmp (argv[1], "2"))
        print_2 ();
    if (!strcmp (argv[1], "3"))
        print_3 ();
    if (!strcmp (argv[1], "4"))
        print_3 ();
    return 0;
}

#endif
