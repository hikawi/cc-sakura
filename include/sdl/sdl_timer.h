/**
 * \file sdl/sdl_timer.h
 *
 * C++ wrappers over SDL's timer modules, providing time-related functions.
 */

#pragma once

#include <cstdint>

namespace sdl
{

/**
 * Retrieves the current ticks of the system in milliseconds.
 *
 * \returns the current ticks
 */
uint64_t get_ticks() noexcept;

} // namespace sdl
