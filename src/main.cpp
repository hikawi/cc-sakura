#include "engine/text.h"
#define SDL_MAIN_USE_CALLBACKS

#include "engine/engine.h"
#include "sdl/sdl_init.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_timer.h"
#include "sdl/sdl_video.h"

#include <memory>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <version>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // Setup nullptr just as a fallback.
    *appstate = nullptr;

    // Setup SDL before the app.
    sdl::clear_log_priority_prefix();
    sdl::init();

    try
    {
        // Configure because dependency injection bruh.
        std::unique_ptr<sdl::iwindow> window = std::make_unique<sdl::window>(
            APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH, APPLICATION_ORIGINAL_HEIGHT, sdl::window_flags::resizable);
        std::unique_ptr<sdl::irenderer> renderer = std::make_unique<sdl::renderer>(*window, nullptr);

        std::unique_ptr<ccsakura::iapp> app = std::make_unique<ccsakura::app>(std::move(window), std::move(renderer));
        std::unique_ptr<ccsakura::iengine> engine = std::make_unique<ccsakura::engine>(std::move(app));

        // Release for SDL to manage.
        *appstate = engine.release();
    }
    catch (...)
    {
        return SDL_APP_FAILURE;
    }

    // Check for weird shit just to know, because why not.
#ifdef __cpp_lib_constexpr_cmath
    sdl::log_info("C++ feature: constexpr_math supported");
#else
    sdl::log_warn("C++ feature: constexpr_math not found");
#endif

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    ccsakura::engine &engine = *static_cast<ccsakura::engine *>(appstate);

    // Iterate and render.
    if (!engine.iterate(sdl::get_ticks()))
    {
        return SDL_APP_SUCCESS;
    }
    engine.render();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    ccsakura::engine &engine = *static_cast<ccsakura::engine *>(appstate);
    (void)engine;

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
        ccsakura::clear_cache_fonts();
        std::unique_ptr<ccsakura::engine> engine(static_cast<ccsakura::engine *>(appstate));
    }

    sdl::log_debug("Application terminated with result {}", (int)result);
    sdl::quit();
}
