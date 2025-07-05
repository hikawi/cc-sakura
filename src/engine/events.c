#include "engine/events.h"

void handle_key_down_event(AppState *app, SDL_KeyboardEvent e)
{
  switch (e.key)
  {
  case SDLK_W:
    app->keyboard.key_w = true;
    break;
  case SDLK_S:
    app->keyboard.key_s = true;
    break;
  case SDLK_A:
    app->keyboard.key_a = true;
    break;
  case SDLK_D:
    app->keyboard.key_d = true;
    break;
  case SDLK_SPACE:
    app->keyboard.key_space = true;
    break;
  }
}

void handle_key_up_event(AppState *app, SDL_KeyboardEvent e)
{
  switch (e.key)
  {
  case SDLK_W:
    app->keyboard.key_w = false;
    break;
  case SDLK_S:
    app->keyboard.key_s = false;
    break;
  case SDLK_A:
    app->keyboard.key_a = false;
    break;
  case SDLK_D:
    app->keyboard.key_d = false;
    break;
  case SDLK_SPACE:
    app->keyboard.key_space = false;
    break;
  }
}
