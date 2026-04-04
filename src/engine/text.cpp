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

const char *get_font_name(const typeface typeface)
{
    switch (typeface)
    {
    case typeface::daydream:
        return "daydream";
    case typeface::rainy_hearts:
        return "rainyhearts";
    case typeface::unifont:
        return "unifont";
    default:
        return "n/a";
    }
}

bool font::operator==(const font &font) const noexcept
{
    return face == font.face && float_equal(sp, font.sp);
}

bool font::operator!=(const font &font) const noexcept
{
    return face != font.face || !float_equal(sp, font.sp);
}

font_cache::font_cache()
{
    sdl::log_trace("ccsakura::font_cache constructed");
}

font_cache::~font_cache()
{
    clear();
    sdl::log_trace("ccsakura::font_cache destroyed");
}

sdl::ttf::ifont &font_cache::operator[](const font font)
{
    auto result = m_font_map.find(font);
    if (result != m_font_map.end())
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
    m_font_map[font] = std::move(ttf_font);

    sdl::log_debug("Cached a font style: {} - {}sp", get_font_name(font.face), font.sp);
    return *m_font_map.at(font);
}

void font_cache::clear()
{
    m_font_map.clear();
}

ifont_cache *text::s_cache = nullptr;

text::text(const typeface tf, const float sp) : face(tf), sp(sp)
{
    sdl::log_trace("ccsakura::text constructed with typeface {} size {}", get_font_name(tf), sp);
}

text::~text()
{
    sdl::log_trace("ccsakura::text destroyed");
}

std::unique_ptr<sdl::itexture> text::render(const sdl::irenderer &renderer) const noexcept
{
    if (!s_cache)
    {
        sdl::log_critical("Failed to render font with no font cache");
        return nullptr;
    }

    font value_font{face, sp};
    sdl::ttf::ifont &font = (*s_cache)[value_font];
    std::unique_ptr<sdl::isurface> surface = font.render_text_blended(value, color);
    std::unique_ptr<sdl::itexture> texture = renderer.create_texture(*surface.get());
    texture->set_blend_mode(sdl::blend_mode::blend);
    return texture;
}

void text::use_cache(ifont_cache &cache)
{
    s_cache = &cache;
}

} // namespace ccsakura
