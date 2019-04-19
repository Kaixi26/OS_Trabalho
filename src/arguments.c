#include "arguments.h"

#define INITIAL_SIZE 8

struct arguments {
  char** args;
  int argc;
  int size;
};

typedef struct arguments* arguments;

static arguments arg_creat (){
  arguments a;
  if (!(a = calloc (1, sizeof(struct arguments))))
    goto alloc_a_failure;
  a->argc = 0;
  a->size = INITIAL_SIZE;
  if (!(a->args = calloc (a->size, sizeof(char*))))
    goto alloc_args_failure;
  return a;
 alloc_args_failure:
  free (a);
 alloc_a_failure:
  fprintf (stderr, "Error allocating arguments.\n");
  return NULL;
}

static int arg_realloc_double (arguments a){
  int new_size = a->size*2;
  char** new_args = realloc (a->args, sizeof(char*) * new_size);
  if (!new_args){
    fprintf (stderr, "Could not allocate memory.\n");
    goto realloc_failure;
  }else
    a->args = new_args;
  a->size = new_size;
  return 0;
 realloc_failure:
  return 1;
}

static int arg_isFull (const arguments a){
  return a->argc >= a->size;
}

static int arg_realloc_if_full (arguments a){
  if (arg_isFull (a))
    if (!arg_realloc_double (a)){
      fprintf (stderr, "Error reallocating arguments.\n");
      goto realloc_failure;
    }
  return 0;
 realloc_failure:
  return 1;
}

static int arg_append (arguments a, char* new){
  if (!arg_realloc_if_full (a))
    goto realloc_failure;
  a->args[a->argc++] = new;
  return 0;
 realloc_failure:
  return 1;
}

// Calculates the next argument in a (started with non-whitespace) string and allocates it to dest
// Returns allocated string length or -1 in case of error
static int alloc_next_arg (char* dest, const char* str){
  int i;
  for (i=0; dest[i] && !isspace (str[i]); i++)
    ;
  if (i > 0){
    if (!(dest = strndup (str, i)))
      goto alloc_failure;
  }
    else
      dest = NULL;
  return i;
 alloc_failure:
  return -1;
}

static int count_until_non_whitespace (const char* str){
  int i;
  for (i=0; str[i] && isspace (str[i]); i++)
    ;
  return i;
}


#if (TEST_ARGUMENTS)
int main(){
  fprintf (stdout, "nice");
  return 0;
}
#endif
