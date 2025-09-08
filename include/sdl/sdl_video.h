/**
 * \file sdl/sdl_video.h
 *
 * RAII wrappers around SDL's raw pointers for video-related components. Prefer this over raw pointers anytime.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <SDL3/SDL_video.h>
#include <string>

namespace sdl
{

/**
 * Flags for initializing an SDL window.
 */
enum class window_flags : uint64_t
{
    fullscreen = SDL_WINDOW_FULLSCREEN,       ///< the window is open as fullscreen
    hidden = SDL_WINDOW_HIDDEN,               ///< the window is hidden on creation
    resizable = SDL_WINDOW_RESIZABLE,         ///< the window is resizable
    borderless = SDL_WINDOW_BORDERLESS,       ///< applies borderless rendering (no decorations) around the window
    always_on_top = SDL_WINDOW_ALWAYS_ON_TOP, ///< always keeps the window on top
};

/**
 * Combines together multiple window flags.
 *
 * \param lhs the left hand side flag
 * \param rhs the right hand side flag
 * \returns a new combined flag
 */
inline constexpr window_flags operator|(const window_flags lhs, const window_flags rhs)
{
    return static_cast<window_flags>(static_cast<uint64_t>(lhs) | static_cast<uint64_t>(rhs));
}

/**
 * Abstract interface for \ref sdl::window for mocking purposes.
 */
class iwindow
{
  public:
    virtual ~iwindow() = default;

    /**
     * Retrieves the wrapped SDL_Window pointer.
     *
     * \returns the window pointer
     */
    virtual SDL_Window *get() const noexcept = 0;
};

/**
 * Wrapper for SDL_Window, the struct used as an opaque handle to a window.
 */
class window : public iwindow
{
  public:
    /**
     * Initializes a new window with the provided parameters.
     *
     * \param title the title of the window
     * \param width the width of the window
     * \param height the height of the window
     * \param flags the window flags
     * \see sdl::window_flags
     */
    window(const std::string &title, const int width, const int height, const window_flags flags);
    ~window() override;

    SDL_Window *get() const noexcept override;

  private:
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> m_window;
};

} // namespace sdl
