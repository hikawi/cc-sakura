/**
 * \file engine/signal.cpp
 *
 * Implementations for signals that are passed down to scenes.
 */

#include <engine/signal.h>
#include <variant>

namespace ccsakura
{

isignal::isignal(const uint64_t timestamp) : timestamp(timestamp)
{
}

signal_type isignal::get_type() const noexcept
{
    return signal_type::undefined;
}

std::unique_ptr<isignal> isignal::wrap(const sdl::event &ev) noexcept
{
    return std::visit(
        [&ev](auto &&arg) -> std::unique_ptr<isignal>
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, sdl::events::mouse_button>)
            {
                return std::make_unique<signals::mouse>(ev.timestamp, arg);
            }
            else if constexpr (std::is_same_v<T, sdl::events::key>)
            {
                return std::make_unique<signals::key>(ev.timestamp, arg);
            }
            else
            {
                return std::make_unique<signals::undefined>(ev.timestamp);
            }
        },
        ev.data);
}

namespace signals
{

undefined::undefined(const uint64_t timestamp) : isignal(timestamp)
{
}

mouse::mouse(const uint64_t timestamp, const sdl::events::mouse_button &data)
    : isignal(timestamp), button(data.button), down(data.down), clicks(data.clicks), x(data.x), y(data.y)
{
}

signal_type mouse::get_type() const noexcept
{
    return signal_type::mouse;
}

key::key(const uint64_t timestamp, const sdl::events::key &data)
    : isignal(timestamp), scancode(data.scancode), keycode(data.key), keymod(data.mod), raw(data.raw), down(data.down),
      repeat(data.repeat)
{
}

signal_type key::get_type() const noexcept
{
    return signal_type::key;
}

} // namespace signals

} // namespace ccsakura
