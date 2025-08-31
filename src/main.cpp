#define SDL_MAIN_USE_CALLBACKS

#include "app.h"
#include "sdl/sdl_init.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"

#include <cmath>
#include <memory>
#include <numbers>
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
    catch (...)
    {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    const ccsakura::app &app = *static_cast<ccsakura::app *>(appstate);
    const sdl::renderer &renderer = app.get_renderer();

    const float RAINBOW_SPEED = 0.001f;
    static float phase = 0.0f;

    phase += RAINBOW_SPEED;

    // Ensure the phase doesn't grow indefinitely.
    // Cycle it back to 0 when it exceeds 2 * PI to prevent precision issues.
    if (phase > 2.0f * std::numbers::pi_v<float>)
    {
        phase -= 2.0f * std::numbers::pi_v<float>;
    }

    // Calculate the value for each color channel using sine waves with different offsets
    float red = 0.5f * (1.0f + std::sin(phase));
    float green = 0.5f * (1.0f + std::sin(phase + (2.0f * std::numbers::pi_v<float> / 3.0f)));
    float blue = 0.5f * (1.0f + std::sin(phase + (4.0f * std::numbers::pi_v<float> / 3.0f)));

    // Apply the new color to the renderer
    renderer.set_color(red, green, blue, 1.0f);
    renderer.clear();
    renderer.present();

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
