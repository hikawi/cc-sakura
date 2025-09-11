#include "sdl/sdl_surface.h"

#include "sdl/sdl_log.h"

#include <SDL3/SDL_surface.h>

namespace sdl
{

surface::surface(std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> ptr) : m_surface(std::move(ptr))
{
    sdl::log_trace("sdl::surface constructed");
}

surface::~surface()
{
    sdl::log_trace("sdl::surface destroyed");
}

SDL_Surface *surface::get() const noexcept
{
    return m_surface.get();
}

} // namespace sdl
