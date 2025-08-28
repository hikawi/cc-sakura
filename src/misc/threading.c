#include "misc/threading.h"

#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_thread.h"

ThreadData thread_background_init(SDL_ThreadFunction fn, const char *name, void *data)
{
    ThreadData *t_data = SDL_malloc(sizeof(ThreadData));
    t_data->started = SDL_CreateCondition();
    t_data->started_mutex = SDL_CreateMutex();
    t_data->mutex = SDL_CreateMutex();
    t_data->data = data;
    t_data->thread = SDL_CreateThread(fn, name, t_data);

    return *t_data;
}
