#include "item_manager.h"


static int create_item_fd (){
    int fd;
    if ((fd = ITEM_FILE_OPEN(O_RDWR | O_CREAT)) == -1)
        REP_ERR_GOTO_V2 ("Error creating item files.\n", creat_failure);
    if (item_fill_id0 (fd) == -1){
        goto write_failure;
    }
    return fd;
 write_failure:
    close (fd);
 creat_failure:
    return -1;
}

static int open_item_fd (){
    int fd;
    if (FILE_EXISTS (ITEM_FILE_PATH)){
        if ((fd = ITEM_FILE_OPEN(O_RDWR)) == -1)
            REP_ERR_GOTO_V2 ("Error opening item file.\n", open_failure);
    }else{
        if ((fd = create_item_fd ()) == -1)
            goto open_failure;
    }
    return fd;
 open_failure:
    return -1;
}

int open_item_name_fd (){
    int fd;
    if ((fd = ITEM_NAME_FILE_OPEN(O_RDWR | O_CREAT)) == -1)
        REP_ERR_GOTO_V2 ("Error opening item name file.\n", open_failure);
    return fd;
 open_failure:
    return -1;
}

int iman_add (arguments a, int a_depth, int item_fd, int item_name_fd){
    const char* name;
    const char* price_str;
    double price_double;
    item_price_type price;
    int id;
    if (!(name = arg_argv (a, a_depth++)))
        REP_ERR_GOTO_V2 ("Could not add item: couldn't get name.\n", invalid_input_failure);
    if (!(price_str = arg_argv (a, a_depth)))
        REP_ERR_GOTO_V2 ("Could not add item: couldn't get price.\n", invalid_input_failure);
    price_double = atof (price_str);
    if (price_double <= 0)
        REP_ERR_GOTO_V2 ("Could not add item: invalid price.\n", invalid_input_failure);
    price = double_to_item_price (price_double);
    if ((id = item_new_append (price, name, item_fd, item_name_fd)) == -1){
        goto append_failure;
    }
    return id;
 append_failure:
 invalid_input_failure:
    return -1;
}

int iman_set_name (arguments a, int a_depth, int item_fd, int item_name_fd){
    item it = item_read (atoi (arg_argv (a, a_depth++)), item_fd, item_name_fd);
    const char* name = arg_argv (a, a_depth);
    item_name_set (it, name);
    item_write (it, item_fd, item_name_fd);
    return 0;
}

int iman_set_price (arguments a, int a_depth, int item_fd, int item_name_fd){
    item it = item_read (atoi (arg_argv (a, a_depth++)), item_fd, item_name_fd);
    item_price_type price;
    double price_double = atof (arg_argv (a, a_depth));
    if (price_double <= 0)
        goto bad_input_failure;
    price = double_to_item_price(price_double);
    item_price_set (it, price);
    item_write (it, item_fd, item_name_fd);
    return 0;
 bad_input_failure:
    return -1;
}

int iman_print (arguments a, int a_depth, int item_fd, int item_name_fd){
    item it = item_read (atoi (arg_argv (a, a_depth)), item_fd, item_name_fd);
    if (it){
        printf ("%f %s\n", item_price_to_double (item_price (it)), item_name (it));
    }
    item_dest (&it);
    return 0;
}

int iman_arg_parse (arguments a, int item_fd, int item_name_fd){
    int r = -1;
    if (STR_EQ (arg_argv (a, 0), "print")){
        r = iman_print (a, 1, item_fd, item_name_fd);
    }else  if (arg_argc (a) < 3)
        REP_ERR_GOTO_V2 ("Not enought arguments to parse.\n", missing_args_failure);
    if (STR_EQ (arg_argv (a, 0), "i")){
        r = iman_add (a, 1, item_fd, item_name_fd);
    }
    if (STR_EQ (arg_argv (a, 0), "n")){
        r = iman_set_name (a, 1, item_fd, item_name_fd);
    }
    if (STR_EQ (arg_argv (a, 0), "p")){
        r = iman_set_price (a, 1, item_fd, item_name_fd);
    }
    if (r == -1){
        goto bad_input_failure;
    }
    return r;
 bad_input_failure:
 missing_args_failure:
    return -1;
}

#if (ITEM_MANAGER)
int main (){
    int item_fd = open_item_fd ();
    int item_name_fd = open_item_name_fd();
    char buffer[10240];
    int id;
    while (fgets (buffer, 10240, stdin)){
        arguments a = arg_get (buffer);
        if ((id =iman_arg_parse (a, item_fd, item_name_fd)) > 0)
            printf ("%d\n", id);
        arg_dest (&a);
    }
    close (item_fd);
    close (item_name_fd);
    return 0;
}
#endif
