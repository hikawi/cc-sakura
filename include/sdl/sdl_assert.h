/**
 * \file sdl/sdl_assert.h
 *
 * Inline macro assertions made with SDL's assertions to be able to optimize out during release builds.
 */

#pragma once

#include <SDL3/SDL_assert.h>

namespace sdl
{

/**
 * Asserts an expression.
 *
 * This becomes a no-op at release.
 *
 * \param expr the expression to assert
 */
inline constexpr void assert(bool expr) noexcept
{
    SDL_assert(expr);
}

} // namespace sdl
