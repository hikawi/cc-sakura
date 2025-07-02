// engine/engine.h
//
// The main engine component of the game. This should handle
// physics and state updates.

#pragma once

#include "app.h"

/**
 * Make the engine run one iteration of the application.
 */
void engine_iterate(AppState *app);
