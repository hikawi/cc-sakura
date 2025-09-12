#include "sdl/sdl_render.h"

#include "engine/render.h"
#include "engine/vec2d.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_rect.h"
#include "sdl/sdl_surface.h"

#include <memory>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

namespace sdl
{

texture::texture(std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_ptr) : m_texture(std::move(texture_ptr))
{
    if (!m_texture)
    {
        sdl::log_critical("Unable to setup SDL texture: {}", SDL_GetError());
        throw std::runtime_error("Unable to setup SDL Texture");
    }

    sdl::log_trace("sdl::texture constructed");
}

texture::~texture()
{
    sdl::log_trace("sdl::texture destroyed");
}

SDL_Texture *texture::get() const noexcept
{
    return m_texture.get();
}

blend_mode texture::get_blend_mode() const noexcept
{
    SDL_BlendMode mode;
    SDL_GetTextureBlendMode(m_texture.get(), &mode);
    return static_cast<blend_mode>(mode);
}

void texture::set_blend_mode(const blend_mode mode) const noexcept
{
    SDL_SetTextureBlendMode(m_texture.get(), static_cast<SDL_BlendMode>(mode));
}

renderer::renderer(const iwindow &window, const char *name)
    : m_renderer(SDL_CreateRenderer(window.get(), name), SDL_DestroyRenderer)
{
    if (!m_renderer)
    {
        sdl::log_critical("Unable to setup SDL renderer: {}", SDL_GetError());
        throw std::runtime_error("Unable to setup SDL renderer");
    }

    sdl::log_trace("sdl::renderer constructed with window {} named {}", SDL_GetWindowTitle(window.get()),
                   SDL_GetRendererName(m_renderer.get()));
    m_name = SDL_GetRendererName(m_renderer.get());
}

SDL_Renderer *renderer::get() const noexcept
{
    return m_renderer.get();
}

std::unique_ptr<itexture> renderer::create_texture(const pixel_format format, const texture_access access, const int w,
                                                   const int h) const noexcept
{
    sdl::log_trace("sdl::renderer {} creating a texture {}x{}", m_name, w, h);
    SDL_Texture *txt = SDL_CreateTexture(m_renderer.get(), static_cast<SDL_PixelFormat>(format),
                                         static_cast<SDL_TextureAccess>(access), w, h);
    auto texture_ptr = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)>(txt, SDL_DestroyTexture);
    return std::make_unique<texture>(std::move(texture_ptr));
}

std::unique_ptr<itexture> renderer::create_texture(const sdl::isurface &surface) const noexcept
{
    SDL_Texture *txt = SDL_CreateTextureFromSurface(m_renderer.get(), surface.get());
    auto txt_ptr = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)>(txt, SDL_DestroyTexture);
    return std::make_unique<texture>(std::move(txt_ptr));
}

void renderer::render_texture(const texture_render_options &options) const noexcept
{
    auto ptr_or_null = [&](const auto &opt) -> auto * { return opt.has_value() ? &*opt : nullptr; };
    auto rect_shift = [&](sdl::frect rect) -> SDL_FRect
    {
        ccsakura::shift_origin(rect, options.m_render_origin);
        return {rect.x, rect.y, rect.w, rect.h};
    };
    auto point_shift = [&](ccsakura::vec2d pos) -> SDL_FPoint
    {
        sdl::frect dstrect = options.m_dstrect.value();
        sdl::frect center(static_cast<float>(pos.x), static_cast<float>(pos.y), dstrect.w, dstrect.h);
        ccsakura::shift_origin(center, options.m_render_origin);
        return {center.x, center.y};
    };

    std::optional<SDL_FRect> srcrect = options.m_srcrect.transform(rect_shift);
    std::optional<SDL_FRect> dstrect = options.m_dstrect.transform(rect_shift);
    std::optional<SDL_FPoint> center;
    if (dstrect && options.m_origin)
    {
        center = options.m_origin.transform(point_shift);
    }

    SDL_RenderTextureRotated(m_renderer.get(), options.m_texture.get(), ptr_or_null(srcrect), ptr_or_null(dstrect),
                             options.m_rotation, ptr_or_null(center), static_cast<SDL_FlipMode>(options.m_flip_mode));
}

void renderer::set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const noexcept
{
    sdl::log_trace("sdl::renderer {} color set to {}, {}, {}, {}", m_name, r, g, b, a);
    SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a);
}

void renderer::set_color(const float r, const float g, const float b, const float a) const noexcept
{
    sdl::log_trace("sdl::renderer {} color set to {}, {}, {}, {}", m_name, r, g, b, a);
    SDL_SetRenderDrawColorFloat(m_renderer.get(), r, g, b, a);
}

void renderer::clear() const noexcept
{
    sdl::log_trace("sdl::renderer {} clear", m_name);
    SDL_RenderClear(m_renderer.get());
}

void renderer::present() const noexcept
{
    sdl::log_trace("sdl::renderer {} present", m_name);
    SDL_RenderPresent(m_renderer.get());
}

renderer::~renderer()
{
    sdl::log_trace("sdl::renderer destroyed");
}

texture_render_options::texture_render_options(sdl::itexture &texture) : m_texture(texture)
{
}

texture_render_options &texture_render_options::srcrect(const sdl::frect rect) noexcept
{
    m_srcrect = rect;
    return *this;
}

texture_render_options &texture_render_options::dstrect(const sdl::frect rect) noexcept
{
    m_dstrect = rect;
    return *this;
}

texture_render_options &texture_render_options::dst(const ccsakura::vec2d pos) noexcept
{
    float w = 0, h = 0;

    w = static_cast<float>(m_texture.get()->w);
    h = static_cast<float>(m_texture.get()->h);

    if (m_srcrect.has_value())
    {
        sdl::frect rect = m_srcrect.value();
        w = rect.w;
        h = rect.h;
    }

    m_dstrect = {static_cast<float>(pos.x), static_cast<float>(pos.y), w, h};
    return *this;
}

texture_render_options &texture_render_options::render_origin(const ccsakura::render_origin origin) noexcept
{
    m_render_origin = origin;
    return *this;
}

texture_render_options &texture_render_options::origin(const ccsakura::vec2d pos) noexcept
{
    m_origin = pos;
    return *this;
}

texture_render_options &texture_render_options::rotate(const double angle) noexcept
{
    m_rotation = angle;
    return *this;
}

texture_render_options &texture_render_options::flip(const sdl::flip flipmode) noexcept
{
    m_flip_mode = flipmode;
    return *this;
}

} // namespace sdl
