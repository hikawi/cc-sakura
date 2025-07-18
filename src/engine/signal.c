#include "engine/signal.h"

#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

Signal *signal_init(SignalType type)
{
    Signal *signal = SDL_malloc(sizeof(Signal));

    signal->timestamp = SDL_GetTicks();
    signal->type      = type;

    return signal;
}

void signal_destroy(Signal *signal)
{
    SDL_free(signal);
}
