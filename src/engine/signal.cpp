/**
 * \file engine/signal.cpp
 *
 * Implementations for signals that are passed down to scenes.
 */

#include "engine/signal.h"

#include <variant>

namespace ccsakura
{

isignal::isignal(const uint64_t timestamp) : timestamp(timestamp)
{
}

std::type_index isignal::type() const noexcept
{
    return typeid(isignal);
}

bool isignal::is_cancelled() const noexcept
{
    return m_cancelled;
}

void isignal::set_cancelled(const bool cancelled) noexcept
{
    m_cancelled = cancelled;
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
            else if constexpr (std::is_same_v<T, sdl::events::quit>)
            {
                return std::make_unique<signals::quit>(ev.timestamp);
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

std::type_index undefined::type() const noexcept
{
    return typeid(undefined);
}

quit::quit(const uint64_t timestamp) : isignal(timestamp)
{
}

std::type_index quit::type() const noexcept
{
    return typeid(quit);
}

mouse::mouse(const uint64_t timestamp, const sdl::events::mouse_button &data)
    : isignal(timestamp), button(data.button), down(data.down), clicks(data.clicks), x(data.x), y(data.y)
{
}

std::type_index mouse::type() const noexcept
{
    return typeid(mouse);
}

key::key(const uint64_t timestamp, const sdl::events::key &data)
    : isignal(timestamp), scancode(data.scan), keycode(data.key), keymod(data.mod), raw(data.raw), down(data.down),
      repeat(data.repeat)
{
}

std::type_index key::type() const noexcept
{
    return typeid(key);
}

} // namespace signals

} // namespace ccsakura
