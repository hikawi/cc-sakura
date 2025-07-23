/**
 * \file misc/threading.h
 *
 * Helper functions for multithread programming using SDL3's interface.
 */

#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_thread.h"

/**
 * The helper threading data struct.
 *
 * There are various warnings and requirements that are expected by the user to make sure everything runs smoothly and
 * seamlessly with the loading scene.
 */
typedef struct
{
    /**
     * The condition for the "started" state.
     *
     * The thread itself has the responsibility to notify conditions properly, using `SDL_SignalCondition()`, with
     * the `started_mutex` when it starts.
     */
    SDL_Condition *started;

    /**
     * The mutex for the \ref started condition.
     *
     * This mutex must be locked before condition signaling can be called.
     */
    SDL_Mutex *started_mutex;

    /**
     * The mutex lock for the entire thread.
     *
     * Unlocks this mutex to signal to the loading scene that the thread is finished with its job.
     */
    SDL_Mutex *mutex;

    /**
     * The actual opaque thread handle.
     */
    SDL_Thread *thread;

    /**
     * The userdata transferred in by the background call.
     */
    void *data;
} ThreadData;

/**
 * Creates a new thread in the background, and returns a condition variable.
 * This condition is signaled when the thread is started.
 *
 * The thread function should return int and have a parameter `void *`, that
 * should be casted to (`ThreadData *`). The thread has the responsibility to
 * notify conditions properly, and free up the data after use.
 *
 * \param fn the thread function, must returns `int` and have one parameter `void *`
 * \param name the thread's name, can be as much as needed, and will be truncated to the OS's demands
 * \param data custom user data to pass into the child thread
 * \returns a thread data information block
 */
ThreadData thread_background_init(SDL_ThreadFunction fn, const char *name, void *data);
