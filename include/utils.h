/**
 * \file utils.h
 *
 * Unclassified utility functions and classes that do not belong anywhere.
 */

#pragma once

#include <string>

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

namespace std
{

template <> struct hash<std::string>
{
    size_t operator()(const std::string &s) const noexcept
    {
        size_t hash = 1469598103934665603ULL;
        for (const char c : s)
        {
            hash ^= static_cast<unsigned char>(c);
            hash *= 1099511628211ULL;
        }
        return hash;
    }
};

} // namespace std
