#include "engine/physics.h"
#include "misc/vector.h"

Vector2 apply_movement(Vector2 pos, Vector2 dir, double spd)
{
    dir = normalize_vector2(dir);
    dir = scale_vector2(dir, spd);
    return add_vector2(pos, dir);
}

void apply_velocity(Vector2 *pos, Vector2 *velo)
{
    (void)pos;
    (void)velo;
}
