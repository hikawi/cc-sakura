/**
 * \file misc/mathex.h
 *
 * Extensions for mathematical operations that for some reasons don't exist yet in SDL's library.
 */

#pragma once

#include <stdbool.h>

/**
 * Checks if two floats are equal in a close enough manner.
 *
 * \param a first number
 * \param b second number
 * \returns true if both are equal enough
 * \see feq
 */
bool feqf(float a, float b);

/**
 * Checks if two doubles are equal in a close enough manner.
 *
 * \param a first double
 * \param b second double
 * \returns true if both are equal enough
 * \see feqf
 */
bool feq(double a, double b);
