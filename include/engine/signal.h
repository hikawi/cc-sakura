/**
 * \file engine/signal.h
 *
 * Interface for signals that are passed down to scenes.
 */

#pragma once

#include "sdl/sdl_events.h"

#include <cstdint>
#include <memory>

namespace ccsakura
{

/**
 * Enumerable list of signal types for the engine's signal bus dispatcher.
 */
enum class signal_type
{
    undefined,
    mouse,
    key,
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
     * Gets the type of the signal.
     */
    virtual signal_type get_type() const noexcept;

    /**
     * Wraps a raw sdl::event into an instance of isignal.
     */
    static std::unique_ptr<isignal> wrap(const sdl::event &ev) noexcept;

    const uint64_t timestamp;
};

namespace signals
{

/// Represents an unknown signal that could not be parsed correctly.
struct undefined : isignal
{
    undefined(const uint64_t);
};

/// Represents a mouse signal.
struct mouse : isignal
{
    mouse(const uint64_t timestamp, const sdl::events::mouse_button &data);
    signal_type get_type() const noexcept override;

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
    signal_type get_type() const noexcept override;

    sdl::scancode scancode;
    sdl::keycode keycode;
    sdl::keymod keymod;
    uint16_t raw;
    bool down;
    bool repeat;
};

} // namespace signals

} // namespace ccsakura
