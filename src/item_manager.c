#include "item_manager.h"

#define ITEM_FILE_PATH "ARTIGOS"
#define ITEM_NAME_FILE_PATH "STRINGS"
#define ITEM_FILE_OPEN(xflags) open (ITEM_FILE_PATH, xflags, 0666)
#define ITEM_NAME_FILE_OPEN(xflags) open (ITEM_NAME_FILE_PATH, xflags, 0666)
#define FILE_EXISTS(path) !access (path, F_OK)

int create_item_fd (){
  int fd;
  if ((fd = ITEM_FILE_OPEN(O_RDWR | O_CREAT)) == -1){
    fprintf (stderr, "Error creating item files.\n");
    goto creat_failure;
  }
  if (item_fill_id0 (fd) == -1){
    goto write_failure;
  }
  return fd;
 write_failure:
  close (fd);
 creat_failure:
  return -1;
}

int open_item_fd (){
  int fd;
  if (FILE_EXISTS (ITEM_FILE_PATH)){
    if ((fd = ITEM_FILE_OPEN(O_RDWR)) == -1){
      fprintf (stderr, "Error opening item file.\n");
      goto open_failure;
    }
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
  if ((fd = ITEM_NAME_FILE_OPEN(O_RDWR | O_CREAT)) == -1){
    fprintf (stderr, "Error opening item name file.\n");
    goto open_failure;
  }
  return fd;
 open_failure:
  return -1;
}

int iman_add (arguments a, int a_depth, int item_fd, int item_name_fd){
  const char* name;
  const char* price_str;
  int price;
  int id;
  if (!(name = arg_argv (a, a_depth++))){
    fprintf (stderr, "Could not add item: couldn't get name.\n");
    goto invalid_input_failure;
  }
  if (!(price_str = arg_argv (a, a_depth))){
    fprintf (stderr, "Could not add item: couldn't get price.\n");
    goto invalid_input_failure;
  }
  price = atoi (price_str);
  if (price < 0 || (price == 0 && !STR_EQ(price_str,"0"))){
    fprintf (stderr, "Could not add item: invalid price.\n");
    goto invalid_input_failure;
  }
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
  int price = atoi (arg_argv (a, a_depth));
  if (price < 0 || (price == 0 && !STR_EQ (arg_argv (a, a_depth), "0")))
    goto bad_input_failure;
  item_price_set (it, price);
  item_write (it, item_fd, item_name_fd);
  return 0;
 bad_input_failure:
  return -1;
}

int iman_print (arguments a, int a_depth, int item_fd, int item_name_fd){
  item it = item_read (atoi (arg_argv (a, a_depth)), item_fd, item_name_fd);
  if (it){
    printf ("%d %s\n", item_price (it), item_name (it));
  }
  item_dest (&it);
  return 0;
}

int iman_arg_parse (arguments a, int item_fd, int item_name_fd){
  int r = -1;
  if (STR_EQ (arg_argv (a, 0), "print")){
    r = iman_print (a, 1, item_fd, item_name_fd);
  }else  if (arg_argc (a) < 3){
    fprintf (stderr, "Not enought arguments to parse.\n");
    goto missing_args_failure;
  }
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
