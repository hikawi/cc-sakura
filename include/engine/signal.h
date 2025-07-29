/**
 * \file engine/signal.h
 *
 * The engine's signal-based event system. This is to separate the terminology used by SDL which is an "App Event". An
 * event that the engine created and wanted to propagate through its system is then called a **Signal**.
 */

#pragma once

#include "engine/collision.h"
#include "SDL3/SDL_stdinc.h"

/**
 * Represents the type of a signal.
 */
typedef enum
{
    /**
     * Placeholder for an empty signal.
     *
     * This signal will always be ignored and should be ignored by implementations.
     */
    SIGNAL_NONE,

    /**
     * A collision has happened between two colliders.
     *
     * This signal will be checked and propagated through the scenes after a physical step.
     *
     * \see CollisionSignal
     */
    SIGNAL_COLLISION,
    SIGNAL_WINDOW_RESIZED, ///< The user has resized their window. \see WindowResizeSignal
} SignalType;

/**
 * Represents a collision signal.
 *
 * \see SIGNAL_COLLISION
 */
typedef struct
{
    CollisionPair pair; ///< The pair of colliders that collided
    Collision info;     ///< Information on how that collision happened
} CollisionSignal;

/**
 * Represents a signal called when the window is resized.
 *
 * \see SIGNAL_WINDOW_RESIZED
 */
typedef struct
{
    int w;    ///< The original X position
    int h;    ///< The original Y position
    int relx; ///< The relative movement compared to the original X
    int rely; ///< The relative movement compared to the original Y
} windowResizeSignal;

/**
 * Represents a signal of the engine.
 *
 * This is basically just an event. It's named signal to differentiate it between SDL events and the Engine events.
 */
typedef struct
{
    SignalType type;  ///< The signal's type, basically event type
    Uint64 timestamp; ///< The signal's timestamp, when it happened
    union
    {
        CollisionSignal collision;
        windowResizeSignal window_resize;
    };
} Signal;

/**
 * Creates a new signal with the provided type.
 *
 * \warning the caller is responsible for free-ing or destroying the signal after use
 * \param type the signal type
 * \returns a new signal allocated
 */
Signal *signal_init(SignalType type);

/**
 * Destroys a signal.
 *
 * \param signal to destroy
 */
void signal_destroy(Signal *signal);
