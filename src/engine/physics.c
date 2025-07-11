#include "engine/physics.h"
#include "misc/vector.h"

Vector2 apply_movement(Vector2 pos, Vector2 dir, double spd)
{
    dir = vector2_norm(dir);
    dir = vector2_scale(dir, spd);
    return vector2_add(pos, dir);
}

void apply_velocity(Vector2 *pos, Vector2 *velo)
{
    (void)pos;
    (void)velo;
}
