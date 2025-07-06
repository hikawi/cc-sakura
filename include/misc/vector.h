// misc/vector.h
//
// Mathematical operations on vectors. These are not C++ vectors
// that are array lists.

#pragma once

/**
 * Represents a vector of coordinates (x, y). THis may be used as
 * directional vectors or as a 2D point.
 */
#include <stdbool.h>
typedef struct
{
    double x;
    double y;
} Vector2;

/**
 * Makes a new 2D vector.
 */
Vector2 make_vector2(double x, double y);

/**
 * Adds two vectors and returns the result as a newly created vector.
 */
Vector2 add_vector2(Vector2 lhs, Vector2 rhs);

/**
 * Subtracts too vectors and returns the result as a newly created vector.
 */
Vector2 subtract_vector2(Vector2 lhs, Vector2 rhs);

/**
 * Negates a vector.
 */
Vector2 negate_vector2(Vector2 vec);

/**
 * Scales a 2D vector with a scalar.
 */
Vector2 scale_vector2(Vector2 vec, double scalar);

/**
 * Rotates a 2D vector around the origin by an angle in radians.
 *
 * I don't know why but sometimes this does not preserve length, I may be
 * stupid but last implementation I made needs to be normalized AFTER
 * rotation.
 */
Vector2 rotate_vector2(Vector2 vec, double angle);

/**
 * Rotates a 2D vector around the origin by an angle,
 * with its sin and cos precalculated.
 *
 * This is usually used when there is need to rotate multiple
 * vectors in the same angle.
 */
Vector2 rotate_vector2_sincos(Vector2 vec, double sin, double cos);

/**
 * On segment made by A->B, find an arbitrary point Q that is the closest
 * in distance to P.
 */
Vector2 closest_point_on_segment(Vector2 a, Vector2 b, Vector2 p);

/**
 * Calculates the dot product of two vectors.
 */
double dot_vector2(Vector2 a, Vector2 b);

/**
 * Projects a vector `from` onto the vector `to`.
 */
Vector2 project_vector2(Vector2 from, Vector2 to);

/**
 * Finds the rotation of a vector compared to the X axis.
 */
double get_rotation(Vector2 vec);

/**
 * Calculates the length of a 2D vector.
 *
 * This uses some CPU cycles for sqrt, use sparingly.
 */
double length_vector2(Vector2 vec);

/**
 * Calculates the length squared of a 2D vector.
 *
 * This is to mostly conserve CPU cycles for calculating the SQRT if not needed.
 */
double length_squared_vector2(Vector2 vec);

/**
 * Normalize a 2D vector. Does nothing if the vector is already length 1.
 */
Vector2 normalize_vector2(Vector2 vec);

/**
 * Checks if two vectors are equal. This uses epsilon for floating point
 * comparisons.
 */
bool equal_vector2(Vector2 a, Vector2 b);
