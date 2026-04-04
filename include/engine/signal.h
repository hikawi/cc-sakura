/**
 * \file engine/signal.h
 *
 * Interface for signals that are passed down to scenes.
 */

#pragma once

#include "sdl/sdl_events.h"

#include <compare>
#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>
#include <typeindex>

namespace ccsakura
{

/**
 * Enumerable list of signal types for the engine's signal bus dispatcher.
 */
enum class signal_type
{
    undefined, ///< unknown signal type
    mouse,     ///< mouse button or motion event
    key,       ///< keyboard key event
    quit,      ///< application quit request
};

/**
 * Represents a base signal to pass down to engine.
 */
class isignal
{
  public:
    isignal(const uint64_t);
    virtual ~isignal() = default;

    /**
     * Wraps a raw sdl::event into an instance of isignal.
     *
     * \param ev the SDL event to wrap
     * \return a unique pointer to the wrapped signal, or nullptr if unsupported
     */
    static std::unique_ptr<isignal> wrap(const sdl::event &ev) noexcept;

    virtual std::type_index type() const noexcept;

    bool is_cancelled() const noexcept;
    void set_cancelled(const bool cancelled) noexcept;

    const uint64_t timestamp;

  private:
    bool m_cancelled{false};
};

namespace signals
{

/// Represents an unknown signal that could not be parsed correctly.
struct undefined : isignal
{
    undefined(const uint64_t);

    std::type_index type() const noexcept override;
};

/// Represents a quit signal.
struct quit : isignal
{
    quit(const uint64_t timestamp);
    std::type_index type() const noexcept override;
};

/// Represents a mouse signal.
struct mouse : isignal
{
    mouse(const uint64_t timestamp, const sdl::events::mouse_button &data);

    std::type_index type() const noexcept override;

    uint8_t button;
    bool down;
    uint8_t clicks;
    float x;
    float y;
};

/// Represents a key signal.
struct key : isignal
{
    key(const uint64_t timestamp, const sdl::events::key &data);

    std::type_index type() const noexcept override;

    sdl::scancode scancode;
    sdl::keycode keycode;
    sdl::keymod keymod;
    uint16_t raw;
    bool down;
    bool repeat;
};

} // namespace signals

/**
 * Defines the execution order and "strength" of signal listeners.
 * Lower priorities have less impact and run earlier in the chain.
 * Higher priorities are "stronger" and can override or finalize the signal state.
 */
enum class listener_priority : uint8_t
{
    lowest,  ///< lowest priority, runs first
    low,     ///< low priority
    normal,  ///< standard priority
    high,    ///< high priority
    highest, ///< highest priority, runs last
    monitor  ///< special priority for monitoring signals without modification
};

class signal_listener
{
  public:
    template <typename T>
        requires std::derived_from<std::remove_cvref_t<T>, isignal>
    signal_listener(const listener_priority priority, const uint64_t id, const std::function<void(T &)> callback)
        : m_callback([callback](isignal &signal) { callback(static_cast<T &>(signal)); }), m_priority(priority),
          m_id(id), m_type_index(typeid(T))
    {
    }

    void operator()(isignal &signal) const
    {
        m_callback(signal);
    }

    std::strong_ordering operator<=>(const signal_listener &other) const
    {
        if (m_priority != other.m_priority)
        {
            return m_priority <=> other.m_priority;
        }
        return m_id <=> other.m_id;
    }

    bool operator==(const signal_listener &other) const
    {
        return m_id == other.m_id;
    }

    uint64_t get_id() const noexcept
    {
        return m_id;
    }

    listener_priority get_priority() const noexcept
    {
        return m_priority;
    }

    std::type_index get_type_index() const noexcept
    {
        return m_type_index;
    }

  private:
    const std::function<void(isignal &)> m_callback;
    const listener_priority m_priority;
    const uint64_t m_id;
    const std::type_index m_type_index;
};

} // namespace ccsakura
