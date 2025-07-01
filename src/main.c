// main.c
//
// The main entrypoint of the program, any events handling, movements, app state
// updating should be delegated to other source files. Do not do actual handling
// in this file.

#include "SDL3/SDL_init.h"
#define SDL_MAIN_USE_CALLBACKS

#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "app.h"
#include "chars/sakura.h"
#include "render/renderer.h"
#include <stdio.h>

/**
 * Calls when the program starts.
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
  SDL_SetAppMetadata(APPLICATION_NAME, APPLICATION_VERSION, APPLICATION_IDENTIFIER);

  init_window_and_renderer();
  init_sakura();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
  render_test_screen();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  // Always quit the app after.
  if (event->type == SDL_EVENT_QUIT)
  {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  destroy_window_and_renderer();
  destroy_sakura();

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "CLosing application with result %d", result);
}
