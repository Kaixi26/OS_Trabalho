#include "files.h"
#include <stdio.h>


char* files_client_path (long id){
    char* buffer;
    if ((buffer = calloc (1024, sizeof(char))) == NULL)
        return NULL;
    snprintf (buffer, 1024, "%s%ld%s", CLIENT_PRE_PATH, id, PIPES_EXTENSION);
    return buffer;
}
