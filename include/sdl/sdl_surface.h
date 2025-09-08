/**
 * \file sdl/sdl_surface.h
 *
 * Includes RAII wrappers for SDL surfaces rendering.
 */

#pragma once

#include "sdl/sdl_render.h"

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

    /**
     * Renders the surface using the provided renderer.
     *
     * \param renderer the renderer to use
     */
    virtual void render(const sdl::irenderer &renderer) const noexcept = 0;
};

/**
 * Concrete implementation for a SDL surface.
 */
class surface : public isurface
{
  public:
    /**
     * Takes ownership of an SDL_Surface pointer.
     *
     * \param surface the surface pointer to take ownership of
     */
    surface(std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface);

    void render(const sdl::irenderer &renderer) const noexcept override;

  private:
    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> m_surface;
};

} // namespace sdl
