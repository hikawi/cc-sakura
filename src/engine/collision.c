#include "engine/collision.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "misc/vector.h"
#include <stdlib.h>
#include <string.h>

/**
 * GPT-generated for different colors based on collision types.
 */
SDL_Color get_collision_type_debug_color(CollisionType type)
{
  switch (type)
  {
  case COLLISION_SOLID:
    return (SDL_Color){0, 255, 0, 128}; // Green
  case COLLISION_DYNAMIC:
    return (SDL_Color){0, 0, 255, 128}; // Blue
  case COLLISION_SENSOR:
    return (SDL_Color){255, 255, 0, 128}; // Yellow
  case COLLISION_HITBOX:
    return (SDL_Color){255, 0, 0, 128}; // Red
  case COLLISION_HURTBOX:
    return (SDL_Color){255, 0, 255, 128}; // Magenta
  case COLLISION_GHOST:
    return (SDL_Color){160, 32, 240, 100}; // Purple
  case COLLISION_ONE_WAY:
    return (SDL_Color){128, 64, 0, 128}; // Brownish
  case COLLISION_DEBUG_ZONE:
    return (SDL_Color){255, 255, 255, 64}; // White
  default:
    return (SDL_Color){0, 0, 0, 128};
  }
}

/**
 * Checks collisions between 2 AABBs
 * using X-aligned collision and Y-aligned collision.
 */
bool check_collision_aabb_aabb(AABBCollider c1, AABBCollider c2)
{
  return (c1.x - c1.w / 2 <= c2.x + c2.w / 2) && (c1.x + c1.w / 2 >= c2.x - c2.w / 2) && (c1.y - c1.h / 2 <= c2.y + c2.h / 2) && (c1.y + c1.h / 2 >= c2.y - c2.h / 2);
}

/**
 * Checks collisions between an AABB and a Circle, by finding the closest
 * point on the AABB to the circle, then calculate the distance from circle's
 * center. If it's in the radius, it's colliding.
 */
bool check_collision_aabb_circle(AABBCollider c1, CircleCollider c2)
{
  double closest_x = SDL_clamp(c2.x, c1.x - c1.w / 2, c1.x + c1.w / 2);
  double closest_y = SDL_clamp(c2.y, c1.y - c1.h / 2, c1.y + c1.h / 2);

  double dx = closest_x - c2.x, dy = closest_y - c2.y;
  return dx * dx + dy * dy <= c2.r * c2.r;
}

/**
 * Checks collisions between two circles. This checks by checking whether the
 * two centers are close enough.
 */
bool check_collision_circle_circle(CircleCollider c1, CircleCollider c2)
{
  double dx = c2.x - c1.x, dy = c2.y - c1.y;
  return dx * dx + dy * dy <= c1.r * c1.r + c2.r * c2.r;
}

/**
 * Checks collisions between an AABB and a capsule. This separates the capsule into
 * 3 parts, an AABB in the middle and 2 circles. It collides when any of them collide
 * with the AABB.
 */
bool check_collision_aabb_capsule(AABBCollider c1, CapsuleCollider c2)
{
  (void)c1;
  (void)c2;

  // Calculate the direction towards the center to pull ends in.
  // Since our P1 and P2 refer to the entire capsule, not just the "shaft".
  Vector2 p1dir = add_vector2(c2.p2, scale_vector2(c2.p1, -1));
  p1dir = normalize_vector2(p1dir);
  Vector2 p2dir = scale_vector2(p1dir, -1);

  // First we setup a rectangle check, which is more of like a rotated rectangle.
  // Clamp the AABB center to the capsule endpoints.
  Vector2 shaft_start = add_vector2(c2.p1, scale_vector2(p1dir, c2.r));
  Vector2 shaft_end = add_vector2(c2.p2, scale_vector2(p2dir, c2.r));
  double closest_x = SDL_clamp(c1.x, SDL_min(shaft_start.x, shaft_end.x), SDL_max(shaft_start.x, shaft_end.x));
  double closest_y = SDL_clamp(c1.y, SDL_min(shaft_start.y, shaft_end.y), SDL_max(shaft_start.y, shaft_end.y));
  double dx = closest_x - c1.x, dy = closest_y - c1.y;
  bool shaft_hit = dx * dx + dy * dy <= c2.r * c2.r;

  // Then check the semicircles.
  CircleCollider semi1;
  semi1.r = c2.r;
  semi1.x = c2.p1.x + p1dir.x * c2.r;
  semi1.y = c2.p1.y + p1dir.y * c2.r;

  CircleCollider semi2;
  semi2.r = c2.r;
  semi2.x = c2.p2.x + p2dir.x * c2.r;
  semi2.y = c2.p2.y + p2dir.y * c2.r;

  return shaft_hit || check_collision_aabb_circle(c1, semi1) || check_collision_aabb_circle(c1, semi2);
}

bool check_collision(Collider *c1, Collider *c2)
{
  switch (c1->collider_type)
  {
  case COLLIDER_TYPE_AABB:
    switch (c2->collider_type)
    {
    case COLLIDER_TYPE_CAPSULE:
      return check_collision_aabb_capsule(c1->aabb, c2->capsule);
    case COLLIDER_TYPE_CIRCLE:
      return check_collision_aabb_circle(c1->aabb, c2->circle);
    default:
      break;
    }
    break;
  case COLLIDER_TYPE_CAPSULE:
    switch (c2->collider_type)
    {
    case COLLIDER_TYPE_AABB:
      return check_collision_aabb_capsule(c2->aabb, c1->capsule);
    default:
      break;
    }
    break;
  case COLLIDER_TYPE_CIRCLE:
    switch (c2->collider_type)
    {
    case COLLIDER_TYPE_AABB:
      return check_collision_aabb_circle(c2->aabb, c1->circle);
    default:
      break;
    }
    break;
  default:
    break;
  }

  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Colliders %s and %s can't be checked since the check unimplemented.",
               c1->name, c2->name);
  return false;
}

ColliderList *create_collider_list(void)
{
  ColliderList *list = malloc(sizeof(ColliderList));
  list->capacity = 10;
  list->length = 0;
  list->list = malloc(sizeof(Collider *) * (unsigned long)list->capacity);
  return list;
}

void add_collider_to_list(ColliderList *list, Collider *collider)
{
  if (list->length >= list->capacity)
  {
    Collider **new_list = malloc(sizeof(Collider *) * (unsigned long)list->capacity * 2);
    memcpy(new_list, list->list, sizeof(Collider *) * (unsigned long)list->length);
    free(list->list);
    list->list = new_list;
  }

  list->list[list->length++] = collider;
}

void remove_collider_at_index(ColliderList *list, int idx)
{
  if (idx < 0 || idx >= list->length)
    return;

  // Move if only it's not the last element. Because if it's the last,
  // just decrementing the length is enough.
  if (idx != list->length - 1)
  {
    memmove(&list->list[idx], &list->list[idx + 1],
            (unsigned long)(list->length - idx - 1) * sizeof(Collider *));
  }
  list->length--;
}

void remove_collider_from_list(ColliderList *list, Collider *collider)
{
  for (int i = 0; i < list->length; i++)
  {
    if (list->list[i] == collider)
    {
      remove_collider_at_index(list, i);
      break;
    }
  }
}

void remove_collider_by_name(ColliderList *list, const char *name)
{
  for (int i = 0; i < list->length; i++)
  {
    if (strcmp(name, list->list[i]->name) == 0)
    {
      remove_collider_at_index(list, i);
      break;
    }
  }
}

void destroy_collider_list(ColliderList *list)
{
  free(list->list);
  free(list);
}
