/**
 * \file sdl/sdl_stdinc.h
 *
 * Utility functions of SDL to interface it with the system (no need for C runtime, but we use C++ anyway).
 */

#pragma once

#include <cstdint>
#include <SDL3/SDL_stdinc.h>
#include <type_traits>

namespace sdl
{

/**
 * Computes a MurMur3 32-bit hash of a pointer.
 *
 * \param value the pointer to hash
 * \param seed the seed to start with, 0 is fine
 * \returns a 32-bit murmur hash
 */
template <typename T> uint32_t murmur3(const T &value, const uint32_t seed = 0)
{
    static_assert(std::is_trivially_copyable<T>(), "murmur3 can only hash trivially copyable types");
    return SDL_murmur3_32((void *)&value, sizeof(T), seed);
}

} // namespace sdl
