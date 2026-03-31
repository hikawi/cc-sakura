#include "sdl/sdl_events.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>

namespace sdl
{

event event::wrap(const SDL_Event &e) noexcept
{
    switch (e.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        return {e.button.timestamp, events::mouse_button{
                                        .button = e.button.button,
                                        .down = e.button.down,
                                        .clicks = e.button.clicks,
                                        .padding = e.button.padding,
                                        .x = e.button.x,
                                        .y = e.button.y,
                                    }};
    case SDL_EVENT_QUIT:
        return {e.quit.timestamp, events::quit{}};
    }

    return {SDL_GetTicksNS(), events::unknown{}};
}

} // namespace sdl
