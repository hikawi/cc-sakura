#include "engine/text.h"

#include "sdl/sdl_iostream.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_storage.h"
#include "sdl/sdl_ttf.h"
#include "utils.h"

#include <optional>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>
#include <unordered_map>

namespace ccsakura
{

static std::unordered_map<font, std::unique_ptr<sdl::ttf::font>> font_cache{};

void clear_cache_fonts()
{
    font_cache.clear();
}

std::string get_font_path(const typeface typeface)
{
    switch (typeface)
    {
    case typeface::daydream:
        return "assets/font/daydream.ttf";
    case typeface::rainy_hearts:
        return "assets/font/rainyhearts.ttf";
    case typeface::unifont:
        return "assets/font/unifont.ttf";
    default:
        return "";
    }
}

sdl::ttf::font &get_cached_font(const font font)
{
    auto result = font_cache.find(font);
    if (result != font_cache.end())
    {
        return *(result->second);
    }

    const std::string path = get_font_path(font.face);
    std::unique_ptr<sdl::istorage> storage = sdl::open_title_storage();
    storage->wait_until_ready();

    const auto ttf_buf = storage->read_file(path);
    if (!ttf_buf)
    {
        sdl::log_error("Failed to read font file");
        throw std::runtime_error("Failed to read font file to cache a font");
    }

    std::shared_ptr<sdl::iiostream> io_buf = std::make_shared<sdl::iostream>(std::move(ttf_buf.value()));
    std::unique_ptr<sdl::ttf::font> ttf_font = std::make_unique<sdl::ttf::font>(io_buf, font.sp);
    ttf_font->set_hint(sdl::ttf::font_hint::light_subpixel);
    font_cache[font] = std::move(ttf_font);

    sdl::log_debug("Cached a font style: {} - {}sp", static_cast<int>(font.face), font.sp);
    return *font_cache.at(font);
}

bool font::operator==(const font &font) const noexcept
{
    return face == font.face && float_equal(sp, font.sp);
}

bool font::operator!=(const font &font) const noexcept
{
    return face != font.face || !float_equal(sp, font.sp);
}

text::text(const font font, const std::string text) : m_font(font), m_text(text), m_color(0, 0, 0, 255), m_pos(0, 0)
{
    sdl::log_trace("ccsakura::text constructed with text {}", text);
}

text::~text()
{
    sdl::log_trace("ccsakura::text destroyed");
}

void text::set_color(const sdl::color color) noexcept
{
    m_color = color;
}

void text::set_text(const std::string text) noexcept
{
    m_text = text;
}

void text::set_position(const ccsakura::vec2d pos) noexcept
{
    m_pos = pos;
}

std::unique_ptr<sdl::itexture> text::render(const sdl::irenderer &renderer) const noexcept
{
    sdl::ttf::font &font = get_cached_font(m_font);
    std::unique_ptr<sdl::isurface> surface = font.render_text_blended(m_text, m_color);
    std::unique_ptr<sdl::itexture> texture = renderer.create_texture(*surface);
    texture->set_blend_mode(sdl::blend_mode::blend);
    return texture;
}

} // namespace ccsakura
