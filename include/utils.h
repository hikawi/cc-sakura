/**
 * \file utils.h
 *
 * Unclassified utility functions and classes that do not belong anywhere.
 */

#pragma once

#include <cstdlib>

namespace ccsakura
{

static constexpr double epsilon = 1e-9;

/**
 * Checks if two floats are equal if they are similar enough to an epsilon.
 *
 * \param a the first value
 * \param b the second value
 * \returns true if they are equal enough
 */
inline constexpr bool float_equal(const float a, const float b) noexcept
{
    return std::abs(a - b) < static_cast<float>(epsilon);
}

/**
 * Checks if two doubles are equal if they are similar enough to an epsilon.
 *
 * \param a the first value
 * \param b the second value
 * \returns true if they are equal enough
 */
inline constexpr bool double_equal(const double a, const double b) noexcept
{
    return std::abs(a - b) < epsilon;
}

} // namespace ccsakura
