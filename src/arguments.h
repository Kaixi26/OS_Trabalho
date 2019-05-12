#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

#include "debug.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// estrutura que guarda os argumentos
typedef struct arguments* arguments;

// faz parse dos argumentos numa string
arguments arg_get (const char*);
// liberta o espaço alocado para os argumentos
void arg_free (arguments);

// retorna o número de argumentos
int arg_argc (arguments);
// retorna o argumento na posição dada
const char* arg_argv (arguments, int index);

#endif
