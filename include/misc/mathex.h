// misc/mathex.h
//
// Math extensions for various mathematical operations.

#pragma once

#include <stdbool.h>

#define EPSILON 0.00001

/**
 * Checks if two floats are equal in a close enough manner.
 */
bool feqf(float a, float b);

/**
 * Checks if two doubles are equal in a close enough manner.
 */
bool feq(double a, double b);
