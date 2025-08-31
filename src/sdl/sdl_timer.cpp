#include "sdl/sdl_timer.h"

#include <cstdint>
#include <SDL3/SDL_timer.h>

namespace sdl
{

uint64_t get_ticks() noexcept
{
    return SDL_GetTicks();
}

} // namespace sdl
