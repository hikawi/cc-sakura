#include "sdl/sdl_log.h"

#include <SDL3/SDL_log.h>

namespace sdl
{

void clear_log_priority_prefix()
{
    for (uint32_t i = 0; i < SDL_LOG_PRIORITY_COUNT; i++)
    {
        SDL_SetLogPriorityPrefix(static_cast<SDL_LogPriority>(i), "");
    }
}

} // namespace sdl
