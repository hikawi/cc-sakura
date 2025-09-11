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
#include <vector>

namespace sdl
{

/**
 * Virtual interface for an SDL IOStream.
 */
class iiostream
{
  public:
    virtual ~iiostream() = default;

    /**
     * Retrieves the IOStream wrapped by this wrapper.
     *
     * \returns the wrapped pointer
     */
    virtual SDL_IOStream *get() const noexcept = 0;
};

/**
 * Wraps an SDL's IOStream.
 */
class iostream : public iiostream
{
  public:
    /**
     * Wraps the provided memory as if it's an IOStream.
     *
     * \param span the span for a byte array to view into
     */
    iostream(std::vector<std::byte> span);

    /**
     * Creates a new empty IOStream to dynamic memory.
     */
    iostream();

    ~iostream();

    SDL_IOStream *get() const noexcept override;

  private:
    std::vector<std::byte> m_span;
    std::unique_ptr<SDL_IOStream, bool (*)(SDL_IOStream *)> m_iostream;
};

} // namespace sdl
