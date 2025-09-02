/**
 * \file utils.h
 *
 * Unclassified utility functions and classes that do not belong anywhere.
 */

#pragma once

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
bool float_equal(const float a, const float b) noexcept;

/**
 * Checks if two doubles are equal if they are similar enough to an epsilon.
 *
 * \param a the first value
 * \param b the second value
 * \returns true if they are equal enough
 */
bool double_equal(const double a, const double b) noexcept;

} // namespace ccsakura
