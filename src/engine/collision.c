#include "engine/collision.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "misc/mathex.h"
#include "misc/vector.h"
#include <math.h>

/**
 * GPT-generated for different colors based on collision types.
 */
SDL_Color collision_get_debug_color(CollisionType type)
{
    switch (type)
    {
    case COLLISION_SOLID:
        return (SDL_Color){0, 255, 0, 64}; // Green
    case COLLISION_DYNAMIC:
        return (SDL_Color){0, 0, 255, 64}; // Blue
    case COLLISION_SENSOR:
        return (SDL_Color){255, 255, 0, 64}; // Yellow
    case COLLISION_HITBOX:
        return (SDL_Color){255, 0, 0, 64}; // Red
    case COLLISION_HURTBOX:
        return (SDL_Color){255, 0, 255, 64}; // Magenta
    case COLLISION_GHOST:
        return (SDL_Color){160, 32, 240, 64}; // Purple
    case COLLISION_ONE_WAY:
        return (SDL_Color){128, 64, 0, 64}; // Brownish
    case COLLISION_DEBUG_ZONE:
        return (SDL_Color){255, 255, 255, 64}; // White
    default:
        return (SDL_Color){0, 0, 0, 64};
    }
}

/**
 * Calculates the collision that occurred between two AABBs. It simply
 * returns an info with `is_colliding = false` if not colliding.
 */
Collision collision_aabb_aabb(const AABBCollider c1, const AABBCollider c2)
{
    Collision info = {.is_colliding = false, .normal = {0, 0}, .depth = 0};

    // Calculate corner points of each collider.
    double min_x1 = c1.x - c1.w / 2, max_x1 = c1.x + c1.w / 2;
    double min_y1 = c1.y - c1.h / 2, max_y1 = c1.y + c1.h / 2;
    double min_x2 = c2.x - c2.w / 2, max_x2 = c2.x + c2.w / 2;
    double min_y2 = c2.y - c2.h / 2, max_y2 = c2.y + c2.h / 2;

    // Calculate overlap.
    double overlap_x = SDL_min(max_x1, max_x2) - SDL_max(min_x1, min_x2);
    double overlap_y = SDL_min(max_y1, max_y2) - SDL_max(min_y1, min_y2);

    // Calculate the area that they overlap.
    // A collision occurs when both overlaps are greater than 0.
    if (overlap_x > 0 && overlap_y > 0)
    {
        info.is_colliding = true;

        // After this, we check for the collision normal, which is the "vector"
        // that we can use to push one collider out with the least force
        // possible.
        //
        // For example, if Sakura jumps to the right and hit the wall, goes in
        // the wall a little, then her collision would be due to slamming her
        // face to the right, the normal would need to push her out of the wall,
        // meaning the wall's normal is to the left.
        //
        // Imagine overlap_x and overlay_y to be the sides of the overlapping
        // rectangle. The shorter side would decide the direction of the normal
        // vector.

        if (overlap_x < overlap_y)
        {
            info.depth = overlap_x;

            // Okay, we know to push c2 left or right, but where? Depends on
            // where c2 is. If c1 is to the left of c2, push right, and vice
            // versa.

            if (c1.x < c2.x)
                info.normal.x = 1.0;
            else
                info.normal.x = -1.0;

            info.normal.y = 0.0;
        }
        else
        {
            info.depth = overlap_y;

            // Okay, we know to push c2 up or down, but where? Depends on
            // where c2 is. If c1 is to the top of c2, push down, and vice
            // versa.

            if (c1.y < c2.y)
                info.normal.y = 1.0;
            else
                info.normal.y = -1.0;

            info.normal.x = 0.0;
        }
    }

    return info;
}

/**
 * Calculates the collision between an AABB and a Circle.
 */
Collision collision_aabb_circle(AABBCollider c1, CircleCollider c2)
{
    Collision info = {.is_colliding = false, .depth = 0};

    // First, we clamp to find the closest point on the AABB to the circle.
    double closest_x = SDL_clamp(c2.x, c1.x - c1.w / 2, c1.x + c1.w / 2);
    double closest_y = SDL_clamp(c2.y, c1.y - c1.h / 2, c1.y + c1.h / 2);

    // The collision happens when dx^2 + dy^2 <= r^2
    double dx = c2.x - closest_x, dy = c2.y - closest_y;
    double dist = dx * dx + dy * dy;
    if (dist > c2.r * c2.r)
        return info;

    info.is_colliding = true;

    // We separate cases to calculate the normal.
    // First special case, the closest point is the circle center (distance 0).
    if (dist == 0)
    {
        // For this case, we calculate the distance between the center
        // to each of the AABB sides, the shortest one is the normal.
        double left = c2.x - (c1.x - c1.w / 2);
        double right = (c1.x + c1.w / 2) - c2.x;
        double top = c2.y - (c1.y - c1.h / 2);
        double bottom = (c1.y + c1.h / 2) - c2.y;

        // Find the minimum and its index.
        double vals[4] = {left, right, top, bottom};
        double min = left;
        int idx = 0;

        for (int i = 1; i < 4; i++)
        {
            if (vals[i] < min)
            {
                min = vals[i];
                idx = i;
            }
        }

        // The depth is the minimum + radius, and we find the normal based on
        // which index was chosen. Also like it's stupid head-scratching here,
        // but if an edge is the smallest distance, we push TOWARDS that edge.
        //
        // |-------------|
        // |             |
        // | O           |
        // |             |
        // |-------------|
        //
        // Imagine O is the center of the circle.
        //
        // It's the closest to the left edge, so naturally and intuitively, to
        // "resolve" the collision, we need to push the circle left.
        //
        // How much to push by? That is the depth needed to push it out. From
        // the left edge to the center, is the "min" distance. And the rest of
        // the other half circle is radius. To push it fully out, it is min +
        // radius.
        //
        // In what direction? We just check that with what edge is the closest.

        info.depth = min + c2.r;
        switch (idx)
        {
        case 0:
            info.normal = (Vector2){.x = -1, .y = 0};
            break;
        case 1:
            info.normal = (Vector2){.x = 1, .y = 0};
            break;
        case 2:
            info.normal = (Vector2){.x = 0, .y = -1};
            break;
        case 3:
            info.normal = (Vector2){.x = 0, .y = 1};
            break;
        default:
            break;
        }
    }
    else
    {
        // Normal case, the normal vector is the vector that points from
        // the circle center to the closest point.
        double sqrt_dist = SDL_sqrt(dist);
        info.normal.x = dx / sqrt_dist;
        info.normal.y = dy / sqrt_dist;
        info.depth = c2.r - sqrt_dist;
    }

    return info;
}

/**
 * Finds the closest point on the AABB to an arbitrary point P by clamping
 * its dimensions.
 */
Vector2 get_closest_point_on_aabb_to_point(AABBCollider aabb, Vector2 p)
{
    Vector2 q;
    q.x = SDL_clamp(p.x, aabb.x - aabb.w / 2, aabb.x + aabb.w / 2);
    q.y = SDL_clamp(p.y, aabb.y - aabb.h / 2, aabb.y + aabb.h / 2);
    return q;
}

/**
 * Checks collisions between two circles.
 */
Collision collision_circle_circle(CircleCollider c1, CircleCollider c2)
{
    Collision info = {.is_colliding = false, .depth = 0};

    // Two circles collide when the distance between their centers are
    // greater than the sum of their radii.
    // But we calculate it as a vector that points outwards from c1, we take c2
    // - c1.
    double dx = c2.x - c1.x, dy = c2.y - c1.y;
    if (dx * dx + dy * dy > SDL_pow(c1.r + c2.r, 2))
        return info;

    // If the distance is 0, somehow, then we just push randomly up idk.
    if (dx * dx + dy * dy == 0)
    {
        info.normal = (Vector2){.x = 0, .y = -1};
        info.depth = c1.r + c2.r;
        return info;
    }

    // Now it's colliding.
    info.is_colliding = true;

    // I think this is a lot simpler, the depth is how much it's collided by
    // (distance - r1 - r2 = -depth), and the normal vector is already
    // calculated.
    double dist = SDL_sqrt(dx * dx + dy * dy);
    info.depth = -(dist - c1.r - c2.r);
    info.normal.x = dx / dist;
    info.normal.y = dy / dist;

    return info;
}

/**
 * Calculates the collision of two OBBs.
 */
Collision collision_obb_obb(OBBCollider c1, OBBCollider c2)
{
    Collision info = {.is_colliding = false, .depth = 0};

    // Step 1. Get some data for the local axes in each OBB. We're using
    // the Separating Axis Theorem. Calculate the local axes for each OBB.
    Vector2 local_x1 =
        (Vector2){.x = SDL_cos(c1.angle), .y = SDL_sin(c1.angle)};
    Vector2 local_y1 =
        (Vector2){.x = -SDL_sin(c1.angle), .y = SDL_cos(c1.angle)};
    Vector2 local_x2 =
        (Vector2){.x = SDL_cos(c2.angle), .y = SDL_sin(c2.angle)};
    Vector2 local_y2 =
        (Vector2){.x = -SDL_sin(c2.angle), .y = SDL_cos(c2.angle)};

    // Step 2. Find the vector that crosses the two centers.
    Vector2 center = (Vector2){.x = c2.x - c1.x, .y = c2.y - c1.y};

    // Step 3. For each local axes, project all of them onto each other.
    Vector2 axes[4] = {local_x1, local_y1, local_x2, local_y2};
    double overlap = INFINITY;
    int idx = 0;
    for (int i = 0; i < 4; i++)
    {
        // Project c1.
        double u1 = SDL_fabs(vector2_dot(axes[i], local_x1)) * c1.w / 2;
        double v1 = SDL_fabs(vector2_dot(axes[i], local_y1)) * c1.h / 2;
        double r1 = u1 + v1;

        // Project c2.
        double u2 = SDL_fabs(vector2_dot(axes[i], local_x2)) * c2.w / 2;
        double v2 = SDL_fabs(vector2_dot(axes[i], local_y2)) * c2.h / 2;
        double r2 = u2 + v2;

        // Project center.
        double s = SDL_fabs(vector2_dot(center, axes[i]));
        if (s > r1 + r2)
        {
            // Separated axis found.
            // No collision.
            return info;
        }

        // Track the axis with the minimum overlap.
        if (r1 + r2 - s < overlap)
        {
            overlap = r1 + r2 - s;
            idx = i;
        }
    }

    // Now we got the collision.
    // The dot product represents their directions relative to each other,
    // a negative product means opposite directions.
    // Our center vector already points us correctly (C1 -> C2)
    if (vector2_dot(center, axes[idx]) < 0)
        info.normal = vector2_scale(axes[idx], -1);
    else
        info.normal = axes[idx];
    info.normal = vector2_norm(info.normal);

    // The depth is the overlap
    info.depth = overlap;

    return info;
}

/**
 * Checks collisions between an AABB and an OBB. This simply turns the AABB into
 * an OBB with angle of 0.
 */
Collision collision_aabb_obb(AABBCollider c1, OBBCollider c2)
{
    OBBCollider c3;
    c3.x = c1.x;
    c3.y = c1.y;
    c3.w = c1.w;
    c3.h = c1.h;
    c3.angle = 0;

    return collision_obb_obb(c3, c2);
}

/**
 * Checks collisions between an OBB and a circle collider. This transforms the
 * circle into the OBB's local coordinates system, then treat the OBB as an AABB
 * in that system.
 */
Collision get_collision_obb_circle(OBBCollider c1, CircleCollider c2)
{
    // Step 1. Transform the circle into local coordinate space
    Vector2 new_center = {.x = c2.x - c1.x, .y = c2.y - c1.y};
    new_center = vector2_rot(new_center, -c1.angle);

    // Step 2. Treat the OBB as an AABB and check collisions.
    // The OBB is IN THE ORIGIN in its own coordinate space.
    AABBCollider aabb;
    aabb.x = 0;
    aabb.y = 0;
    aabb.w = c1.w;
    aabb.h = c1.h;

    CircleCollider circ;
    circ.x = new_center.x;
    circ.y = new_center.y;
    circ.r = c2.r;

    // Step 3. Translate back.
    Collision collision = collision_aabb_circle(aabb, circ);
    if (collision.is_colliding)
    {
        collision.normal = vector2_rot(collision.normal, c1.angle);
    }

    return collision;
}

/**
 * Checks collisions between an AABB and a capsule. This is done by finding
 * the closest point on the capsule's central line segment to the AABB,
 * and the closest point on the AABB to that segment point. Collision occurs
 * if the distance between these two points is less than the capsule's radius.
 */
Collision collision_aabb_capsule(AABBCollider c1, CapsuleCollider c2)
{
    Collision info = {.is_colliding = false, .depth = 0};

    // Step 1. Find the closest point on the capsule to the AABB center.
    // Before doing that we would like to find the true segment (the one at the
    // shaft) not the ones that cover the entire capsule.
    Vector2 segment = vector2_norm(vector2_sub(c2.p1, c2.p2));
    Vector2 new_p1 = vector2_add(c2.p1, vector2_scale(segment, -c2.r));
    Vector2 new_p2 = vector2_add(c2.p2, vector2_scale(segment, c2.r));
    Vector2 p = closest_point_on_segment(new_p1, new_p2,
                                         (Vector2){.x = c1.x, .y = c1.y});

    // Step 2. Find the closest point on the AABB that is the closest to
    // that representative point.
    Vector2 q = get_closest_point_on_aabb_to_point(c1, p);

    // Step 3. Collision happens when the distance between p and q <= c2.r
    // P is on the capsule, Q is on the AABB. To direct away from the AABB, we
    // take p - q.
    Vector2 d = vector2_sub(p, q);
    if (vector2_lensqr(d) > c2.r * c2.r)
    {
        return info;
    }

    info.is_colliding = true;

    // Step 4. Calculate normal and penetration depth. Two cases.
    // dist = 0, the capsule's point segment is fully inside AABB.
    // dist > 0, the capsule kinda just hits the AABB on the side.
    if (vector2_lensqr(d) == 0)
    {
        // I think this is similar to the AABB-Circle one.
        // Find where the minimum distance to each of the AABB edge is.
        // Just copied the code.
        double left = p.x - (c1.x - c1.w / 2);
        double right = (c1.x + c1.w / 2) - p.x;
        double top = p.y - (c1.y - c1.h / 2);
        double bottom = (c1.y + c1.h / 2) - p.y;

        // Find the minimum and its index.
        double vals[4] = {left, right, top, bottom};
        double min = left;
        int idx = 0;

        for (int i = 1; i < 4; i++)
        {
            if (vals[i] < min)
            {
                min = vals[i];
                idx = i;
            }
        }

        // The normal is based on the idx, same as the circle.
        switch (idx)
        {
        case 0:
            info.normal = (Vector2){.x = -1, .y = 0};
            break;
        case 1:
            info.normal = (Vector2){.x = 1, .y = 0};
            break;
        case 2:
            info.normal = (Vector2){.x = 0, .y = -1};
            break;
        case 3:
            info.normal = (Vector2){.x = 0, .y = 1};
            break;
        }

        // Same as circle, depth is r + min
        info.depth = min + c2.r;
    }
    else
    {
        // Normal case.
        info.normal = vector2_norm(d);
        info.depth = c2.r - vector2_len(d);
    }

    return info;
}

/**
 * Retrieves the collision info between an OBB and a Capsule, by rotating
 * the Capsule into OBB's coordinate system and treat the OBB as the AABB.
 */
Collision collision_obb_capsule(OBBCollider c1, CapsuleCollider c2)
{
    // Step 1. Undo the rotation.
    Vector2 obb_center = {.x = c1.x, .y = c1.y};
    Vector2 p1_tl = vector2_sub(c2.p1, obb_center);
    Vector2 p2_tl = vector2_sub(c2.p2, obb_center);
    p1_tl = vector2_rot(p1_tl, -c1.angle);
    p2_tl = vector2_rot(p2_tl, -c1.angle);

    // Step 2. Treat the OBB as an AABB.
    AABBCollider aabb = {.x = 0, .y = 0, .w = c1.w, .h = c1.h};
    CapsuleCollider cap = {.p1 = p1_tl, .p2 = p2_tl, .r = c2.r};

    // Step 3. Check collision, and transform back if needed.
    Collision info = collision_aabb_capsule(aabb, cap);
    if (info.is_colliding)
    {
        info.normal = vector2_rot(info.normal, c1.angle);
    }

    return info;
}

/**
 * Checks the collision between a circle and a capsule.
 */
Collision collision_circle_capsule(CircleCollider c1, CapsuleCollider c2)
{
    Collision info = {.is_colliding = false, .depth = 0};
    Vector2 in = vector2_sub(c2.p1, c2.p2);

    // Special case: degenerate capsule
    if (vector2_len(in) <= c2.r * 2)
    {
        CircleCollider c3 = {
            .x = (c2.p1.x + c2.p2.x) / 2,
            .y = (c2.p1.y + c2.p2.y) / 2,
            .r = c2.r,
        };
        return collision_circle_circle(c1, c3);
    }

    // Step 1. Find the closest point on the capsule closest to the circle's
    // center.
    c2.p1 = vector2_add(c2.p1, vector2_scale(in, -c2.r));
    c2.p2 = vector2_add(c2.p2, vector2_scale(in, c2.r));
    Vector2 p =
        closest_point_on_segment(c2.p1, c2.p2, (Vector2){.x = c1.x, .y = c1.y});

    // Step 2. Calculate the distance between the capsule's segment and the
    // center.
    Vector2 d = vector2_sub(p, (Vector2){.x = c1.x, .y = c1.y});
    if (vector2_lensqr(d) > c1.r + c2.r)
        return info;
    info.is_colliding = true;

    // Step 3. Calculate collision normal.
    if (feq(vector2_lensqr(d), 0))
    {
        // If length is close enough to 0, the center of the circle is on
        // the capsule's segment.
        info.normal = vector2_norm(vector2_rot(in, -M_PI_2));
        info.depth = c1.r + c2.r;
    }
    else
    {
        info.normal = vector2_norm(d);
        info.depth = c1.r + c2.r - vector2_len(d);
    }

    return info;
}

// Collision get_collision_capsule_capsule(CapsuleCollider c1, CapsuleCollider
// c2)
// {
//   Collision info = {0};

//   // Step 1: Shrink segments to exclude caps (requires normalized directions)
//   Vector2 u = subtract_vector2(c1.p2, c1.p1);
//   Vector2 v = subtract_vector2(c2.p2, c2.p1);
//   double u_len = length_vector2(u);
//   double v_len = length_vector2(v);

//   // Refine segments (move inward by radius)
//   Vector2 u_dir = scale_vector2(u, 1.0 / u_len);
//   Vector2 v_dir = scale_vector2(v, 1.0 / v_len);
//   c1.p1 = add_vector2(c1.p1, scale_vector2(u_dir, c1.r));
//   c1.p2 = add_vector2(c1.p2, scale_vector2(u_dir, -c1.r));
//   c2.p1 = add_vector2(c2.p1, scale_vector2(v_dir, c2.r));
//   c2.p2 = add_vector2(c2.p2, scale_vector2(v_dir, -c2.r));

//   // Step 2: Recompute u/v/w after refinement
//   u = subtract_vector2(c1.p2, c1.p1);
//   v = subtract_vector2(c2.p2, c2.p1);
//   Vector2 w = subtract_vector2(c2.p1, c1.p1);

//   // Step 3: Solve for s and t
//   double uu = dot_vector2(u, u);
//   double uv = dot_vector2(u, v);
//   double uw = dot_vector2(u, w);
//   double vv = dot_vector2(v, v);
//   double vw = dot_vector2(v, w);
//   double denominator = uv * uv - uu * vv;

//   // Parallel segments? Fall back to endpoint checks.
//   if (fabs(denominator) < EPSILON)
//   {
//     // (Implement endpoint-segment distance checks here)
//     return info;
//   }

//   double t = SDL_clamp((uv * uw - vw * uu) / denominator, 0.0, 1.0);
//   double s = SDL_clamp((-uw + t * uv) / uu, 0.0, 1.0);

//   // Step 4: Closest points
//   Vector2 p = add_vector2(c1.p1, scale_vector2(u, s));
//   Vector2 q = add_vector2(c2.p1, scale_vector2(v, t));
//   Vector2 d = subtract_vector2(q, p);
//   double dist = length_vector2(d);

//   // Step 5: Collision check
//   if (dist > c1.r + c2.r)
//     return info;
//   info.is_colliding = true;

//   // Step 6: Normal and depth
//   if (dist < EPSILON)
//   {
//     // Segments intersect; use perpendicular to relative direction
//     Vector2 rel_dir = normalize_vector2(subtract_vector2(u, v));
//     info.normal = (Vector2){-rel_dir.y, rel_dir.x}; // 90° rotation
//     info.depth = c1.r + c2.r;
//   }
//   else
//   {
//     info.normal = scale_vector2(d, 1.0 / dist); // Normalize
//     info.depth = c1.r + c2.r - dist;
//   }

//   return info;
// }

Collision collision_check(Collider *c1, Collider *c2)
{
    // NOTE:
    // c1 is the COLLIDED OBJECT.
    // c2 is the COLLIDING OBJECT.

    Collision info = {.is_colliding = false, .depth = 0};

    switch (c1->collider_type)
    {
    case COLLIDER_TYPE_AABB:
        switch (c2->collider_type)
        {
        case COLLIDER_TYPE_CAPSULE:
            return collision_aabb_capsule(c1->aabb, c2->capsule);
        case COLLIDER_TYPE_CIRCLE:
            return collision_aabb_circle(c1->aabb, c2->circle);
        case COLLIDER_TYPE_AABB:
            return collision_aabb_aabb(c1->aabb, c2->aabb);
        case COLLIDER_TYPE_OBB:
            return collision_aabb_obb(c1->aabb, c2->obb);
        }
        break;
    case COLLIDER_TYPE_CAPSULE:
        switch (c2->collider_type)
        {
        case COLLIDER_TYPE_AABB:
            info = collision_aabb_capsule(c2->aabb, c1->capsule);
            info.normal = vector2_neg(info.normal);
            return info;
        case COLLIDER_TYPE_OBB:
            info = collision_obb_capsule(c2->obb, c1->capsule);
            info.normal = vector2_neg(info.normal);
            return info;
        case COLLIDER_TYPE_CIRCLE:
            info = collision_circle_capsule(c2->circle, c1->capsule);
            info.normal = vector2_neg(info.normal);
            return info;
        default:
            break;
        }
        break;
    case COLLIDER_TYPE_CIRCLE:
        switch (c2->collider_type)
        {
        case COLLIDER_TYPE_AABB:
            info = collision_aabb_circle(c2->aabb, c1->circle);
            info.normal = vector2_neg(info.normal);
            return info;
        case COLLIDER_TYPE_CIRCLE:
            return collision_circle_circle(c1->circle, c2->circle);
        case COLLIDER_TYPE_OBB:
            info = get_collision_obb_circle(c2->obb, c1->circle);
            info.normal = vector2_neg(info.normal);
            return info;
        case COLLIDER_TYPE_CAPSULE:
            return collision_circle_capsule(c1->circle, c2->capsule);
        default:
            break;
        }
        break;
    case COLLIDER_TYPE_OBB:
        switch (c2->collider_type)
        {
        case COLLIDER_TYPE_AABB:
            info = collision_aabb_obb(c2->aabb, c1->obb);
            info.normal = vector2_neg(info.normal);
            return info;
        case COLLIDER_TYPE_CIRCLE:
            return get_collision_obb_circle(c1->obb, c2->circle);
        case COLLIDER_TYPE_OBB:
            return collision_obb_obb(c1->obb, c2->obb);
        case COLLIDER_TYPE_CAPSULE:
            return collision_obb_capsule(c1->obb, c2->capsule);
        }
        break;
    default:
        break;
    }

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Colliders %s and %s can't be checked since the check is "
                 "unimplemented.",
                 c1->name, c2->name);
    return info;
}
