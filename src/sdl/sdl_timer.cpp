#include "sdl/sdl_timer.h"

#include <cstdint>
#include <SDL3/SDL_timer.h>

namespace sdl
{

uint64_t get_ticks() noexcept
{
    return SDL_GetTicks();
}

uint64_t get_ticks_ns() noexcept
{
    return SDL_GetTicksNS();
}

void delay(std::chrono::milliseconds duration) noexcept
{
    SDL_Delay(static_cast<uint32_t>(duration.count()));
}

} // namespace sdl
