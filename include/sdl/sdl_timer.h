/**
 * \file sdl/sdl_timer.h
 *
 * C++ wrappers over SDL's timer modules, providing time-related functions.
 */

#pragma once

#include <chrono>
#include <cstdint>

namespace sdl
{

/**
 * Retrieves the current ticks of the system in milliseconds.
 *
 * \returns the current ticks
 */
uint64_t get_ticks() noexcept;

/**
 * Delays the system synchronously for a duration.
 *
 * \param duration the duration to wait
 */
void delay(std::chrono::milliseconds duration) noexcept;

} // namespace sdl
