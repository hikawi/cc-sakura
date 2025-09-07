#include "sdl/sdl_iostream.h"

#include "sdl/sdl_log.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <stdexcept>

namespace sdl
{

iostream::iostream(const std::span<const std::byte> span)
    : m_iostream(SDL_IOFromConstMem(span.data(), span.size_bytes()), SDL_CloseIO)
{
    if (!m_iostream)
    {
        sdl::log_error("Unable to create SDL IOStream: {}", SDL_GetError());
        throw std::runtime_error("Unable to create SDL IOStream from context");
    }
}

iostream::iostream() : m_iostream(SDL_IOFromDynamicMem(), SDL_CloseIO)
{
    if (!m_iostream)
    {
        sdl::log_error("Unable to create SDL IOStream: {}", SDL_GetError());
        throw std::runtime_error("Unable to create SDL IOStream from context");
    }
}

SDL_IOStream *iostream::get() const noexcept
{
    return m_iostream.get();
}

} // namespace sdl
