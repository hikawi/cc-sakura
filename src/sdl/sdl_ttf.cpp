#include "sdl/sdl_ttf.h"

#include "sdl/sdl_iostream.h"
#include "sdl/sdl_log.h"

#include <SDL3/SDL_error.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>

namespace sdl::ttf
{

font_style operator|(const font_style lhs, const font_style rhs) noexcept
{
    const auto result = static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
    return static_cast<font_style>(result);
}

font::font(const sdl::iostream &io, const float sp) : m_font(TTF_OpenFontIO(io.get(), false, sp), TTF_CloseFont)
{
    if (!m_font)
    {
        sdl::log_error("Unable to open font: {}", SDL_GetError());
        throw std::runtime_error("Unable to open font");
    }
}

void font::set_style(const font_style style) const noexcept
{
    TTF_SetFontStyle(m_font.get(), static_cast<TTF_FontStyleFlags>(style));
}

void font::set_size(const float sp) const noexcept
{
    TTF_SetFontSize(m_font.get(), sp);
}

} // namespace sdl::ttf
