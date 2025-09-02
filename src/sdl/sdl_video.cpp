#include "sdl/sdl_video.h"

#include "sdl/sdl_log.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

namespace sdl
{

window::window(const std::string &title, const int width, const int height, const window_flags flags)
    : m_window(SDL_CreateWindow(title.c_str(), width, height, static_cast<uint64_t>(flags)), SDL_DestroyWindow)
{
    if (!m_window)
    {
        sdl::log_critical("Unable to setup SDL window: {}", SDL_GetError());
        throw std::runtime_error("Unable to setup SDL window");
    }

    sdl::log_trace("sdl::window constructed at {}x{}", width, height);
}

SDL_Window *window::get() const
{
    return m_window.get();
}

window::~window()
{
    sdl::log_trace("sdl::window destroyed");
}

} // namespace sdl
