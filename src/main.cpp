#include "sdl/sdl_init.h"
#include "sdl/sdl_log.h"
#define SDL_MAIN_USE_CALLBACKS

#include "app.h"

#include <exception>
#include <memory>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // Setup nullptr just as a fallback.
    *appstate = nullptr;

    // Setup SDL before the app.
    sdl::init();

    try
    {
        std::unique_ptr<ccsakura::app> app = std::make_unique<ccsakura::app>();
        *appstate = app.release();
    }
    catch (std::exception &ex)
    {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    (void)appstate;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void)appstate;

    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Retake ownership
    {
        std::unique_ptr<ccsakura::app> app(static_cast<ccsakura::app *>(appstate));
    }

    sdl::log_debug("Application termintated with result {}", (int)result);
    sdl::quit();
}
