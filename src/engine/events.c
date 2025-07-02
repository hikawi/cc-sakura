#include "engine/events.h"
#include "spr/sakura.h"

void handle_key_down_event(AppState *_, SDL_KeyboardEvent e)
{
  Sakura *skr = get_sakura();

  switch (e.key)
  {
  case SDLK_W:
    skr->is_moving_up = true;
    break;
  case SDLK_S:
    skr->is_moving_down = true;
    break;
  case SDLK_A:
    skr->is_moving_left = true;
    break;
  case SDLK_D:
    skr->is_moving_right = true;
    break;
  }
}

void handle_key_up_event(AppState *_, SDL_KeyboardEvent e)
{
  Sakura *skr = get_sakura();

  switch (e.key)
  {
  case SDLK_W:
    skr->is_moving_up = false;
    break;
  case SDLK_S:
    skr->is_moving_down = false;
    break;
  case SDLK_A:
    skr->is_moving_left = false;
    break;
  case SDLK_D:
    skr->is_moving_right = false;
    break;
  }
}
