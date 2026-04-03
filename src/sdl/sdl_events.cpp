#include "sdl/sdl_events.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>

namespace sdl
{

events::key wrap_event(const SDL_KeyboardEvent &e)
{
    return {
        .scancode = static_cast<sdl::scancode>(e.scancode),
        .key = static_cast<sdl::keycode>(e.key),
        .mod = static_cast<sdl::keymod>(e.mod),
        .raw = e.raw,
        .down = e.down,
        .repeat = e.repeat,
    };
}

events::mouse_button wrap_event(const SDL_MouseButtonEvent &e)
{
    return {
        .button = e.button,
        .down = e.down,
        .clicks = e.clicks,
        .padding = e.padding,
        .x = e.x,
        .y = e.y,
    };
}

event event::wrap(const SDL_Event &e) noexcept
{
    switch (e.type)
    {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        return {e.common.timestamp, wrap_event(e.key)};
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        return {e.common.timestamp, wrap_event(e.button)};
    case SDL_EVENT_QUIT:
        return {e.common.timestamp, events::quit{}};
    }

    return {SDL_GetTicksNS(), events::unknown{}};
}

} // namespace sdl
