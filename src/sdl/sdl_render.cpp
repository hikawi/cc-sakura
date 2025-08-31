#include "sdl/sdl_render.h"

#include "sdl/sdl_log.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

namespace sdl
{

renderer::renderer(const window &window, const char *name)
    : m_renderer(SDL_CreateRenderer(window.get(), name), SDL_DestroyRenderer)
{
    if (!m_renderer)
    {
        sdl::log_critical("Unable to setup SDL renderer: {}", SDL_GetError());
        throw std::runtime_error("Unable to setup SDL renderer");
    }

    sdl::log_trace("sdl::renderer constructed with window {} named {}", SDL_GetWindowTitle(window.get()),
                   SDL_GetRendererName(m_renderer.get()));
}

void renderer::set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const
{
    SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a);
}

void renderer::set_color(const float r, const float g, const float b, const float a) const
{
    SDL_SetRenderDrawColorFloat(m_renderer.get(), r, g, b, a);
}

void renderer::clear() const
{
    SDL_RenderClear(m_renderer.get());
}

void renderer::present() const
{
    SDL_RenderPresent(m_renderer.get());
}

renderer::~renderer()
{
    sdl::log_trace("sdl::renderer destroyed");
}

} // namespace sdl
