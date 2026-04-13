#include "sdl/sdl_texture.h"

#include "sdl/sdl_error.h"
#include "sdl/sdl_log.h"

namespace sdl
{

texture::texture(std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_ptr) : m_texture(std::move(texture_ptr))
{
    if (!m_texture)
    {
        sdl::log_critical("Unable to setup SDL texture: {}", SDL_GetError());
        throw std::runtime_error("Unable to setup SDL Texture");
    }

    sdl::log_verbose("sdl::texture constructed");
}

texture::~texture()
{
    sdl::log_verbose("sdl::texture destroyed");
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
    if (!SDL_SetTextureBlendMode(m_texture.get(), static_cast<SDL_BlendMode>(mode)))
    {
        sdl::log_error("Failed to set texture blend mode: {}", SDL_GetError());
    }
}

scale_mode texture::get_scale_mode() const noexcept
{
    SDL_ScaleMode mode;
    SDL_GetTextureScaleMode(m_texture.get(), &mode);
    return static_cast<scale_mode>(mode);
}

void texture::set_scale_mode(const scale_mode mode) const noexcept
{
    if (!SDL_SetTextureScaleMode(m_texture.get(), static_cast<SDL_ScaleMode>(mode)))
    {
        sdl::log_error("Failed to set texture scale mode: {}", SDL_GetError());
    }
}

uint8_t texture::get_alpha_mod() const noexcept
{
    Uint8 alpha = 255;
    SDL_GetTextureAlphaMod(m_texture.get(), &alpha);
    return static_cast<uint8_t>(alpha);
}

void texture::set_alpha_mod(const uint8_t alpha) const noexcept
{
    if (!SDL_SetTextureAlphaMod(m_texture.get(), alpha))
    {
        log_error("Failed to set texture alpha mod: {}", get_error());
    }
}

sdl::fcolor texture::get_color_mod() const noexcept
{
    float r = 1.0f, g = 1.0f, b = 1.0f;
    SDL_GetTextureColorModFloat(m_texture.get(), &r, &g, &b);
    return {r, g, b, 1.0f};
}

void texture::set_color_mod(float r, float g, float b) const noexcept
{
    SDL_SetTextureColorModFloat(m_texture.get(), r, g, b);
}

} // namespace sdl
