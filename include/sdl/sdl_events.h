/**
 * \file sdl/sdl_events.h
 *
 * Provides raw wrappers for SDL raw events just to look nicer.
 */

#pragma once

#include <SDL3/SDL_events.h>
#include <variant>

namespace sdl
{

namespace events
{

struct mouse_button
{
    uint8_t button; /**< The mouse button index */
    bool down;      /**< true if the button is pressed */
    uint8_t clicks; /**< 1 for single-click, 2 for double-click, etc. */
    uint8_t padding;
    float x; /**< X coordinate, relative to window */
    float y; /**< Y coordinate, relative to window */
};

struct quit
{
};

struct unknown
{
};

}; // namespace events

using event_data = std::variant<events::mouse_button, events::quit, events::unknown>;

class event
{
  public:
    uint64_t timestamp; ///< In nanoseconds.
    event_data data;    ///< Variant data.

    /**
     * Wraps a raw SDL_Event into an instance of sdl::event for easier management.
     */
    static event wrap(const SDL_Event &event) noexcept;
};

} // namespace sdl
