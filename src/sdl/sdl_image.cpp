#include "sdl/sdl_image.h"

#include "sdl/sdl_log.h"
#include "sdl/sdl_surface.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

namespace sdl::image
{

std::unique_ptr<sdl::isurface> load(sdl::iiostream &io)
{
    SDL_Surface *surface = IMG_Load_IO(io.get(), false);
    if (!surface)
    {
        sdl::log_error("Unable to load image from IOStream: {}", SDL_GetError());
        throw std::runtime_error("Unable to load image from IOStream");
    }

    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface_ptr(surface, SDL_DestroySurface);
    return std::make_unique<sdl::surface>(std::move(surface_ptr));
}

} // namespace sdl::image
