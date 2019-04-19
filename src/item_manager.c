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

#if (ITEM_MANAGER)
int main (){
  int item_fd = open_item_fd ();
  int item_name_fd = open_item_name_fd();
  printf ("id = %d\n", item_new_append (1, "13", item_fd, item_name_fd));
  //printf ("id = %d\n", item_new_append (89, "asdf", item_fd, item_name_fd));
  //printf ("id = %d\n", item_new_append (3, "prodas", item_fd, item_name_fd));
  item it = item_read (1000, item_fd, item_name_fd);
  item_dest (&it);
  //printf ("%d %s\n", item_price (it), item_name (it));
  close (item_fd);
  close (item_name_fd);
  return 0;
}
#endif
