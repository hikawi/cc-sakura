#include "engine/collision.h"

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
