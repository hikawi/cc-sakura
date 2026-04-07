/**
 * \file engine/intent.h
 *
 * Defines game intents for decoupling hardware input from game logic.
 */

#pragma once

#include "sdl/sdl_keycode.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

namespace ccsakura
{

/**
 * Enumerable list of game intents.
 * Maps to player actions independent of the physical input device.
 */
enum class intent : uint8_t
{
    move_left,
    move_right,
    move_up,
    move_down,
    count ///< sentinel for array sizing
};

/// Tracks the pressed/released state for each intent.
using intent_state = std::array<bool, static_cast<std::size_t>(intent::count)>;

/**
 * Fluent builder that collects key-to-intent mappings and subscribes them on bind().
 * Obtain via iscene::bind_intents() rather than constructing directly.
 */
class intent_binder
{
  public:
    intent_binder(std::function<void(std::vector<std::pair<sdl::keycode, intent>>)> commit);

    /**
     * Maps a keycode to an intent.
     *
     * \param key the key to listen for
     * \param i the intent to activate
     * \returns this builder for chaining
     */
    intent_binder &map(sdl::keycode key, intent i);

    /**
     * Commits the bindings and subscribes to key signals.
     */
    void bind();

  private:
    std::vector<std::pair<sdl::keycode, intent>> m_bindings;
    std::function<void(std::vector<std::pair<sdl::keycode, intent>>)> m_commit;
};

} // namespace ccsakura
