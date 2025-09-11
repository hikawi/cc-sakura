#include "sdl/sdl_ttf.h"

#include "sdl/sdl_iostream.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_surface.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>
#include <string>

namespace sdl::ttf
{

font_style operator|(const font_style lhs, const font_style rhs) noexcept
{
    const auto result = static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
    return static_cast<font_style>(result);
}

font::font(std::shared_ptr<sdl::iiostream> io, const float sp)
    : m_font(TTF_OpenFontIO(io->get(), false, sp), TTF_CloseFont), m_iostream(std::move(io))
{
    if (!m_font)
    {
        sdl::log_error("Unable to open font: {}", SDL_GetError());
        throw std::runtime_error("Unable to open font");
    }

    sdl::log_trace("sdl::ttf::font constructed with {}sp", sp);
}

font::~font()
{
    sdl::log_trace("sdl::ttf::font destroyed");
}

TTF_Font *font::get() const noexcept
{
    return m_font.get();
}

void font::set_style(const font_style style) const noexcept
{
    TTF_SetFontStyle(m_font.get(), static_cast<TTF_FontStyleFlags>(style));
}

void font::set_size(const float sp) const noexcept
{
    TTF_SetFontSize(m_font.get(), sp);
}

void font::set_hint(const font_hint hint) const noexcept
{
    TTF_SetFontHinting(m_font.get(), static_cast<TTF_HintingFlags>(hint));
}

std::unique_ptr<sdl::isurface> font::render_text_blended(const std::string text, const sdl::color color) const noexcept
{
    SDL_Surface *surface =
        TTF_RenderText_Blended(m_font.get(), text.c_str(), text.length(), {color.r, color.g, color.b, color.a});
    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface_ptr(surface, SDL_DestroySurface);
    return std::make_unique<sdl::surface>(std::move(surface_ptr));
}

} // namespace sdl::ttf
