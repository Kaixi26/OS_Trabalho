#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STR_EQ(str, str2) !(strcmp (str, str2))
#define STR_EQ2(str, str2, str3) (STR_EQ(str, str2) && STR_EQ(str, str3))

typedef struct arguments* arguments;

int arg_argc (arguments);
const char* arg_argv (arguments, int index);
arguments arg_get (const char*);
void arg_dest (arguments*);

#endif
