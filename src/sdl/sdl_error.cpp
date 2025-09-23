#include "sdl/sdl_error.h"

#include <SDL3/SDL_error.h>

namespace sdl
{

std::string get_error()
{
    return SDL_GetError();
}

} // namespace sdl
