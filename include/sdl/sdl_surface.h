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
 * Represents the flipping mode of a surface or a texture.
 */
enum class flip
{
    horizontal = SDL_FLIP_HORIZONTAL,               ///< flips it horizontally
    vertical = SDL_FLIP_VERTICAL,                   ///< flips it vertically
    both = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL, ///< flips both ways
    none = SDL_FLIP_NONE,                           ///< don't flip it
};

/**
 * Represents the scale mode for a surface or a texture, the algorithm to use for scaling
 */
enum class scale_mode
{
    nearest = SDL_SCALEMODE_NEAREST, ///< nearest pixel sampling
    linear = SDL_SCALEMODE_LINEAR,   ///< linear filtering
    invalid = SDL_SCALEMODE_INVALID, ///< invalid scale mode
};

/**
 * Virtual interface for a wrapper of an SDL surface.
 */
class isurface
{
  public:
    virtual ~isurface() = default;

    /**
     * Retrieves the underlying SDL_Surface pointer.
     *
     * \returns the SDL surface pointer
     */
    virtual SDL_Surface *get() const noexcept = 0;
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
    ~surface();

    SDL_Surface *get() const noexcept override;

  private:
    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> m_surface;
};

} // namespace sdl
