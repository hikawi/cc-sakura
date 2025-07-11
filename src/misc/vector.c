#include "misc/vector.h"
#include "SDL3/SDL_stdinc.h"
#include "misc/mathex.h"

Vector2 vector2_make(double x, double y)
{
    Vector2 vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

Vector2 vector2_add(Vector2 lhs, Vector2 rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

Vector2 vector2_sub(Vector2 lhs, Vector2 rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

Vector2 vector2_neg(Vector2 vec)
{
    vec.x = -vec.x;
    vec.y = -vec.y;
    return vec;
}

Vector2 vector2_scale(Vector2 vec, double scalar)
{
    vec.x *= scalar;
    vec.y *= scalar;
    return vec;
}

Vector2 vector2_rot(Vector2 vec, double angle)
{
    double sin = SDL_sin(angle), cos = SDL_cos(angle);
    return vector2_rot_sincos(vec, sin, cos);
}

Vector2 vector2_rot_sincos(Vector2 vec, double sin, double cos)
{
    Vector2 res;
    res.x = vec.x * cos - vec.y * sin;
    res.y = vec.x * sin + vec.y * cos;
    return res;
}

Vector2 vector2_proj(Vector2 from, Vector2 to)
{
    // The projection of vector a onto b, is a vector a' that is in the same
    // direction (or negative) of b, with a length of a projected on b. This
    // scalar is calculated by (a . b) / len(b). Then we multiply with the
    // normalized b.
    Vector2 res;

    double len_b = vector2_len(to);
    double scalar = vector2_dot(from, to) / len_b;
    res.x = to.x * scalar / len_b;
    res.y = to.y * scalar / len_b;

    return res;
}

Vector2 closest_point_on_segment(Vector2 a, Vector2 b, Vector2 p)
{
    Vector2 ab = vector2_sub(b, a);
    Vector2 ap = vector2_sub(p, a);

    double length_sq_ab = vector2_lensqr(ab);

    // Handle case where the segment is length 0
    if (length_sq_ab == 0.0)
    {
        return a;
    }

    // Project P onto the AB
    // This the "alpha parameter" we multiply to the "to" vector after
    // projection to get the projected vector. But we only care about a single
    // point so
    double t = vector2_dot(ap, ab) / length_sq_ab;

    // Clamp t to the [0, 1] range to ensure the point is on the segment
    t = SDL_clamp(t, 0.0, 1.0);

    // Calculate the closest point on the segment
    return (Vector2){a.x + t * ab.x, a.y + t * ab.y};
}

double vector2_dot(Vector2 a, Vector2 b)
{
    return a.x * b.x + a.y * b.y;
}

double vector2_get_rot(Vector2 vec)
{
    return SDL_atan2(vec.y, vec.x);
}

double vector2_len(Vector2 vec)
{
    return SDL_sqrt(vector2_lensqr(vec));
}

double vector2_lensqr(Vector2 vec)
{
    return vec.x * vec.x + vec.y * vec.y;
}

Vector2 vector2_norm(Vector2 vec)
{
    double len_sqr;

    // Do nothing if length is 0 or length is 1.
    if ((len_sqr = vector2_lensqr(vec)) == 0 || len_sqr == 1)
    {
        return vec;
    }

    double len = SDL_sqrt(len_sqr);
    vec.x /= len;
    vec.y /= len;
    return vec;
}

bool vector2_eq(Vector2 a, Vector2 b)
{
    return feq(a.x, b.x) && feq(a.y, b.y);
}
