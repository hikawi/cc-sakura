#include "sdl/sdl_iostream.h"

#include "sdl/sdl_log.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <stdexcept>

namespace sdl
{

iostream::iostream(std::vector<std::byte> span)
    : m_span(std::move(span)), m_iostream(SDL_IOFromConstMem(m_span.data(), m_span.size()), SDL_CloseIO)
{
    if (!m_iostream)
    {
        sdl::log_error("Unable to create SDL IOStream: {}", SDL_GetError());
        throw std::runtime_error("Unable to create SDL IOStream from context");
    }

    sdl::log_trace("sdl::iostream created with constant span of {} bytes", span.size());
}

iostream::iostream() : m_iostream(SDL_IOFromDynamicMem(), SDL_CloseIO)
{
    if (!m_iostream)
    {
        sdl::log_error("Unable to create SDL IOStream: {}", SDL_GetError());
        throw std::runtime_error("Unable to create SDL IOStream from context");
    }

    sdl::log_trace("sdl::iostream created with dynamic memory");
}

iostream::~iostream()
{
    sdl::log_trace("sdl::iostream destroyed");
}

SDL_IOStream *iostream::get() const noexcept
{
    return m_iostream.get();
}

} // namespace sdl
