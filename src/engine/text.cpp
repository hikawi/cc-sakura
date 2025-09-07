#include "engine/text.h"

#include "sdl/sdl_iostream.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_storage.h"
#include "utils.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>

namespace ccsakura
{

static std::unordered_map<font, std::unique_ptr<TTF_Font, void (*)(TTF_Font *)>> font_cache;

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

TTF_Font *get_cached_font(const font font)
{
    auto result = font_cache.find(font);
    if (result != font_cache.end())
    {
        return result->second.get();
    }

    const std::string path = get_font_path(font.typeface);
    std::unique_ptr<sdl::istorage> storage = sdl::open_title_storage();
    storage->wait_until_ready();

    const auto ttf_buf = storage->read_file(path);
    if (!ttf_buf)
    {
        sdl::log_error("Failed to read font file");
        return nullptr;
    }

    sdl::iostream io_buf(ttf_buf.value());
    return nullptr;
}

bool font::operator==(const font &font) const noexcept
{
    return typeface == font.typeface && float_equal(sp, font.sp);
}

bool font::operator!=(const font &font) const noexcept
{
    return typeface != font.typeface || !float_equal(sp, font.sp);
}

text::text(const font font, const std::string text) : m_font(font), m_text(text)
{
}

text &text::operator=(const std::string text) noexcept
{
    m_text = text;
    return *this;
}

void text::render(const sdl::irenderer &renderer) const noexcept
{
    (void)renderer;
}

} // namespace ccsakura
