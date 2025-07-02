#include "engine/physics.h"
#include "misc/vector.h"

Vector2 apply_movement(Vector2 pos, Vector2 dir, double spd, double dt)
{
  dir = normalize_vector2(dir);
  dir = scale_vector2(dir, spd * dt);
  return add_vector2(pos, dir);
}
