#include "app.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_stdinc.h"
#include <string.h>

char *get_resource_path(const char *subpath)
{
    char *path = SDL_calloc(1024, sizeof(char));

    strcat(path, SDL_GetBasePath());
    strcat(path, "/");
    strcat(path, subpath);

    return path;
}
