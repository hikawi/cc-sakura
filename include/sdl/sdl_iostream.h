/**
 * \file sdl/sdl_iostream.h
 *
 * Wrapper for IOStreams similar to standard library's iostream with automatic closure for out-of-scope. I prefer to use
 * this module as SDL provides helpers for dealing with endianness, and it's more precise than standard library's
 * ofstream.
 */

#pragma once

#include <cstddef>
#include <memory>
#include <SDL3/SDL_iostream.h>
#include <span>

namespace sdl
{

/**
 * Wraps an SDL's IOStream.
 */
class iostream
{
  public:
    /**
     * Wraps the provided memory as if it's an IOStream.
     *
     * \param span the span for a byte array to view into
     */
    iostream(const std::span<const std::byte> span);

    /**
     * Creates a new empty IOStream to dynamic memory.
     */
    iostream();

    /**
     * Retrieves the IOStream wrapped by this wrapper.
     *
     * \returns the wrapped pointer
     */
    SDL_IOStream *get() const noexcept;

  private:
    std::unique_ptr<SDL_IOStream, bool (*)(SDL_IOStream *)> m_iostream;
};

} // namespace sdl
