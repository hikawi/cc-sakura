/**
 * \file engine/intent.h
 *
 * Defines game intents for decoupling hardware input from game logic.
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

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

} // namespace ccsakura
