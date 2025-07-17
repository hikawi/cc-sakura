// misc/threading.h
//
// Helper functions for starting background tasks.

#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_thread.h"

typedef struct
{
    SDL_Condition *started;
    SDL_Mutex *started_mutex;

    SDL_Mutex *mutex;
    SDL_Thread *thread;
    void *data;
} ThreadData;

/**
 * Creates a new thread in the background, and returns a condition variable.
 * This condition is signaled when the thread is started.
 *
 * The thread function should return int and have a parameter void *, that
 * should be casted to (ThreadData *). The thread has the responsibility to
 * notify conditions properly, and free up the data after use.
 */
ThreadData thread_background_init(SDL_ThreadFunction fn, const char *name,
                                  void *data);
