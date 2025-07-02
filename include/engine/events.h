// engine/events.h
//
// For specifically handling events that the engine coordinates.

#pragma once

#include "SDL3/SDL_events.h"
#include "app.h"

void handle_key_down_event(AppState *app, SDL_KeyboardEvent e);
void handle_key_up_event(AppState *app, SDL_KeyboardEvent e);
