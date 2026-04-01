#define SDL_MAIN_USE_CALLBACKS

#include "engine/engine.h"
#include "engine/text.h"
#include "sdl/sdl_events.h"
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
        ccsakura::engine_deps deps;
        deps.m_window = std::make_unique<sdl::window>(APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH,
                                                      APPLICATION_ORIGINAL_HEIGHT, sdl::window_flags::resizable);
        deps.m_renderer = std::make_unique<sdl::renderer>(*deps.m_window, nullptr);
        deps.m_app = std::make_unique<ccsakura::app>();
        deps.m_font_cache = std::make_unique<ccsakura::font_cache>();
        deps.m_sprite_cache = std::make_unique<ccsakura::sprite_cache>();

        deps.m_renderer->set_logical_presentation(APPLICATION_LOGICAL_WIDTH, APPLICATION_LOGICAL_HEIGHT,
                                                  sdl::logical_presentation::letterbox);

        std::unique_ptr<ccsakura::iengine> engine = std::make_unique<ccsakura::engine>(std::move(deps));

        // Release for SDL to manage.
        *appstate = engine.release();
    }
    catch (...)
    {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    ccsakura::engine &engine = *static_cast<ccsakura::engine *>(appstate);

    // Iterate and render.
    if (!engine.is_running())
    {
        return SDL_APP_SUCCESS;
    }

    engine.iterate(sdl::get_ticks());
    engine.render();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    ccsakura::engine &engine = *static_cast<ccsakura::engine *>(appstate);
    sdl::event wrapped_event = sdl::event::wrap(*event);

    // This returns true/false for SUCCESS or CONTINUE
    engine.queue_event(std::move(wrapped_event));
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Retake ownership
    {
        std::unique_ptr<ccsakura::engine> engine(static_cast<ccsakura::engine *>(appstate));
    }

    switch (result)
    {
    case SDL_APP_SUCCESS:
        sdl::log_info("Application terminated successfully.");
        break;
    case SDL_APP_FAILURE:
        sdl::log_error("Application terminated with failure.");
        break;
    case SDL_APP_CONTINUE:
        sdl::log_critical("How did you get here?");
        break;
    }

    sdl::quit();
}
