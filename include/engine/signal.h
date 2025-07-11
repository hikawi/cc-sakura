// engine/signal.h
//
// Represents an engine-based event that happens. This is to separate it from
// "Events" which refers to SDL's App Events. An engine-based event, or a
// signal, is similar. A signal is emitted when something happens.

#pragma once

#include "SDL3/SDL_stdinc.h"

/**
 * Represents the type of a signal.
 */
typedef enum
{
    SIGNAL_NONE,
} SignalType;

/**
 * Represents a signal of the engine. This is basically just an event. It's
 * named signal to differentiate it between SDL events and the Engine events.
 */
typedef struct
{
    SignalType type;
    Uint64 timestamp;
} Signal;
