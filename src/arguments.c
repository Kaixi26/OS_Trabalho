#include "arguments.h"

#define INITIAL_SIZE 2

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

void arg_dest (arguments* a){
  if (a && *a){
    for (int i=0; i<(*a)->argc; i++)
      free ((*a)->args[i]);
    free ((*a)->args);
    free (*a);
    *a = NULL;
  }
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
    if (arg_realloc_double (a)){
      fprintf (stderr, "Error reallocating arguments.\n");
      goto realloc_failure;
    }
  return 0;
 realloc_failure:
  return 1;
}

static int arg_append (arguments a, char* new){
  if (arg_realloc_if_full (a))
    goto realloc_failure;
  a->args[a->argc++] = new;
  return 0;
 realloc_failure:
  return 1;
}

// Calculates the next argument between quotes (string must start with quotes)
static char* alloc_next_arg_quote (const char* str, int* i){
  char* ret;
  for (*i = 1; str[*i] && str[*i] != '\''; (*i)++)
    ;
  if (*i > 0 && str[*i]){
    (*i)++; //skip ' character
    if (!(ret = strndup (str+1, (*i)-2)))
      goto alloc_failure;
  }else
    ret = NULL;
  return ret;
  alloc_failure:
  *i = -1;
  return NULL;
}

static char* alloc_next_arg_unquote (const char* str, int* i){
  char* ret;
  for (*i = 0; str[*i] && !isspace(str[*i]); (*i)++)
    ;
  if (*i > 0){
    if (!(ret = strndup (str, *i)))
      goto alloc_failure;
  }else
    ret = NULL;
  return ret;
  alloc_failure:
  *i = -1;
  return NULL;
}

// Calculates the next argument in a (started with non-whitespace) string and allocates it to dest
// Returns allocated string length or -1 in case of error
static char *alloc_next_arg (const char* str, int* i){
  char* ret;
  if (!str[0]){
    goto invalid_arg_failure;
  }
  if (str[0] == '\'')
    ret = alloc_next_arg_quote (str, i);
  else ret = alloc_next_arg_unquote (str, i);
  return ret;
 invalid_arg_failure:
  *i = -1;
  return NULL;
}

static int count_until_non_whitespace (const char* str){
  int i;
  for (i=0; str[i] && isspace (str[i]); i++)
    ;
  return i;
}

arguments arg_get (const char* str){
  arguments a;
  char* tmp = NULL;
  int i=0, r;
  if (!(a = arg_creat ())){
    goto create_failure;
  }
  for (i=0; ;i+=r){
    i += count_until_non_whitespace (str + i);
    if (!str[i])
      break;
    tmp = alloc_next_arg (str + i, &r);
    if (r == -1)
      goto alloc_failure;
    arg_append (a, tmp);
  }
  return a;
 alloc_failure:
  arg_dest (&a);
 create_failure:
  return NULL;
}

int arg_argc (arguments a){
  return a->argc;
}

const char* arg_argv (arguments a, int i){
  if (i >= a->argc)
    return NULL;
  return a->args[i];
}

#if (TEST_ARGUMENTS)
int main(){
  arguments a = arg_get (" 'this is a test'   fjaj fas  this is a test string");
  printf ("%d\n", a->argc);
  for (int i=0; i<a->argc; i++)
    puts (a->args[i]);
  arg_dest (&a);
  return 0;
}
#endif
