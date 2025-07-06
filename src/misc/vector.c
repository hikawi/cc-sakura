#include "misc/vector.h"
#include "SDL3/SDL_stdinc.h"
#include "misc/mathex.h"

Vector2 make_vector2(double x, double y)
{
  Vector2 vec;
  vec.x = x;
  vec.y = y;
  return vec;
}

Vector2 add_vector2(Vector2 lhs, Vector2 rhs)
{
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

Vector2 subtract_vector2(Vector2 lhs, Vector2 rhs)
{
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

Vector2 negate_vector2(Vector2 vec)
{
  vec.x = -vec.x;
  vec.y = -vec.y;
  return vec;
}

Vector2 scale_vector2(Vector2 vec, double scalar)
{
  vec.x *= scalar;
  vec.y *= scalar;
  return vec;
}

Vector2 rotate_vector2(Vector2 vec, double angle)
{
  double sin = SDL_sin(angle), cos = SDL_cos(angle);
  return rotate_vector2_sincos(vec, sin, cos);
}

Vector2 rotate_vector2_sincos(Vector2 vec, double sin, double cos)
{
  Vector2 res;
  res.x = vec.x * cos - vec.y * sin;
  res.y = vec.x * sin + vec.y * cos;
  return res;
}

Vector2 project_vector2(Vector2 from, Vector2 to)
{
  // The projection of vector a onto b, is a vector a' that is in the same direction (or negative)
  // of b, with a length of a projected on b.
  // This scalar is calculated by (a . b) / len(b). Then we multiply with the normalized b.
  Vector2 res;

  double len_b = length_vector2(to);
  double scalar = dot_vector2(from, to) / len_b;
  res.x = to.x * scalar / len_b;
  res.y = to.y * scalar / len_b;

  return res;
}

Vector2 closest_point_on_segment(Vector2 a, Vector2 b, Vector2 p)
{
  Vector2 ab = subtract_vector2(b, a);
  Vector2 ap = subtract_vector2(p, a);

  double length_sq_ab = length_squared_vector2(ab);

  // Handle case where the segment is length 0
  if (length_sq_ab == 0.0)
  {
    return a;
  }

  // Project P onto the AB
  // This the "alpha parameter" we multiply to the "to" vector after projection
  // to get the projected vector. But we only care about a single point so
  double t = dot_vector2(ap, ab) / length_sq_ab;

  // Clamp t to the [0, 1] range to ensure the point is on the segment
  t = SDL_clamp(t, 0.0, 1.0);

  // Calculate the closest point on the segment
  return (Vector2){a.x + t * ab.x, a.y + t * ab.y};
}

double dot_vector2(Vector2 a, Vector2 b)
{
  return a.x * b.x + a.y * b.y;
}

double get_rotation(Vector2 vec)
{
  return SDL_atan2(vec.y, vec.x);
}

double length_vector2(Vector2 vec)
{
  return SDL_sqrt(length_squared_vector2(vec));
}

double length_squared_vector2(Vector2 vec)
{
  return vec.x * vec.x + vec.y * vec.y;
}

Vector2 normalize_vector2(Vector2 vec)
{
  double len_sqr;

  // Do nothing if length is 0 or length is 1.
  if ((len_sqr = length_squared_vector2(vec)) == 0 || len_sqr == 1)
  {
    return vec;
  }

  double len = SDL_sqrt(len_sqr);
  vec.x /= len;
  vec.y /= len;
  return vec;
}

bool equal_vector2(Vector2 a, Vector2 b)
{
  return feq(a.x, b.x) && feq(a.y, b.y);
}
