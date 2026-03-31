/**
 * \file engine/signal.h
 *
 * Interface for signals that are passed down to scenes.
 */

#pragma once

#include <string_view>
#include <variant>

namespace ccsakura
{

/**
 * \brief Base interface for all signals.
 */
class isignal
{
  public:
    virtual ~isignal() = default;

    /**
     * \brief Returns the unique type name of the signal.
     */
    virtual std::string_view type() const noexcept = 0;
};

namespace signals
{

/**
 * An event fired for when a mouse is held down.
 */
struct mouse_down
{
    const float x;
    const float y;
};

/**
 * An event fired for when a mouse is held up.
 */
struct mouse_up
{
    const float x;
    const float y;
};

} // namespace signals

/**
 * Represents a bunch of signal types.
 */
enum class signal_type
{
    mouse_down,
    mouse_up,
};

using signal_data = std::variant<signals::mouse_down, signals::mouse_up>;

/**
 * Represents a signal to be handled.
 */
struct signal
{
    const signal_type type;
    const signal_data data;
};

} // namespace ccsakura
