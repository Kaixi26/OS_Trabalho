#include "files.h"

char* files_client_path (pid_t pid){
    char* buffer;
    if ((buffer = calloc (1024, sizeof(char))) == NULL)
        return NULL;
    snprintf (buffer, 1024, "%s%d%s", CLI_PRE_PATH, pid, PIPES_EXT);
    return buffer;
}
