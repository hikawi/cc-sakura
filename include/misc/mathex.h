// misc/mathex.h
//
// Math extensions for various mathematical operations.

#pragma once

#include <stdbool.h>

#define EPSILON 0.00001

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

#ifndef M_2_PI
#define M_2_PI 0.63661977236758134308
#endif

/**
 * Checks if two floats are equal in a close enough manner.
 */
bool feqf(float a, float b);

/**
 * Checks if two doubles are equal in a close enough manner.
 */
bool feq(double a, double b);
