#include "misc/vector.h"
#include "SDL3/SDL_stdinc.h"

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
