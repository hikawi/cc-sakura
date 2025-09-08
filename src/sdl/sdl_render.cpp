#include "sdl/sdl_render.h"

#include "sdl/sdl_log.h"

#include <memory>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
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

std::unique_ptr<itexture> renderer::create_texture(pixel_format format, texture_access access, int w,
                                                   int h) const noexcept
{
    SDL_Texture *txt = SDL_CreateTexture(m_renderer.get(), static_cast<SDL_PixelFormat>(format),
                                         static_cast<SDL_TextureAccess>(access), w, h);
    auto texture_ptr = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)>(txt, SDL_DestroyTexture);
    return std::make_unique<texture>(std::move(texture_ptr));
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

} // namespace sdl
