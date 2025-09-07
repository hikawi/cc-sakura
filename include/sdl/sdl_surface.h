/**
 * \file sdl/sdl_surface.h
 *
 * Includes RAII wrappers for SDL surfaces rendering.
 */

#pragma once

#include <memory>
#include <SDL3/SDL_surface.h>

namespace sdl
{

/**
 * Virtual interface for a wrapper of an SDL surface.
 */
class isurface
{
  public:
    virtual ~isurface() = default;
};

/**
 * Concrete implementation for a SDL surface.
 */
class surface : public isurface
{
  public:
    surface(std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface);

  private:
    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> m_surface;
};

} // namespace sdl
