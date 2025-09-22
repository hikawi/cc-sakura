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

SDL_IOStream *iostream::get() const noexcept
{
    return m_iostream.get();
}

iostatus iostream::status() const noexcept
{
    return static_cast<iostatus>(SDL_GetIOStatus(m_iostream.get()));
}

void iostream::read_u32_le(uint32_t &out)
{
    if (!SDL_ReadU32LE(m_iostream.get(), &out))
    {
        sdl::log_error("Unable to read U32 LE from IOStream: {}", SDL_GetError());
        throw std::runtime_error("Unable to read U32 LE from IOStream");
    }
}

void iostream::read_u64_le(uint64_t &out)
{
    if (!SDL_ReadU64LE(m_iostream.get(), &out))
    {
        sdl::log_error("Unable to read U64 LE from IOStream: {}", SDL_GetError());
        throw std::runtime_error("Unable to read U64 LE from IOStream");
    }
}

size_t iostream::read(void *buf, size_t len)
{
    size_t bytes_read = SDL_ReadIO(m_iostream.get(), buf, len);
    if (bytes_read == 0 && status() != iostatus::eof)
    {
        sdl::log_error("Can't read from IOStream but not at EOF: {}", SDL_GetError());
        throw std::runtime_error("Can't read from IOStream but not at EOF");
    }

    return bytes_read;
}

iostream::~iostream()
{
    sdl::log_trace("sdl::iostream destroyed");
}

} // namespace sdl
