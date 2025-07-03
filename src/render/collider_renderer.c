#include "render/collider_renderer.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "engine/collision.h"
#include "misc/vector.h"
#include "render/renderer.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * Renders a filled half circle, using a center and a radius,
 * and whether to render it towards the top or bottom.
 */
void render_filled_half_circle(SDL_FPoint center, SDL_FColor fcolor, double radius, bool top)
{
  // Calculate the number of segments.
  // The number of points is segments + 1 + the center.
  // For each segment, we have one triangle.
  unsigned long segments = (unsigned long)SDL_max(6.0, radius / 2);
  SDL_Vertex *vertices = malloc(sizeof(SDL_Vertex) * (segments + 2));
  int *triangles = malloc(sizeof(int) * segments * 3);

  // First and end vertices are calculated from the center.
  // First vertex is the center to draw form.
  vertices[0] = (SDL_Vertex){center, fcolor, {0, 0}};

  // Now, we calculate each vertex, and draw a triangle there.
  for (int i = 0; i < (int)segments + 1; i++)
  {
    // Calculate the start and the from vertices on the arc.
    // We should already got the start index,
    float angle = (SDL_PI_F * i) / segments;
    if (top)
      angle = -angle;

    float x = center.x + SDL_cosf(angle) * (float)radius;
    float y = center.y + SDL_sinf(angle) * (float)radius;
    vertices[i + 1] = (SDL_Vertex){{x, y}, fcolor, {0, 0}};

    // Loop the triangle.
    // Center, start vertex, to vertex
    if (i < (int)segments)
    {
      triangles[i * 3] = 0;
      triangles[i * 3 + 1] = i + 1;
      triangles[i * 3 + 2] = i + 2;
    }
  }

  // Render
  SDL_Renderer *renderer = get_current_renderer();
  SDL_RenderGeometry(renderer, NULL, vertices, (int)segments + 2, triangles, (int)segments * 3);

  // Clean up
  free(vertices);
  free(triangles);
}

/**
 * Renders an AABB collider on the screen.
 */
void render_aabb_collider(Collider *collider)
{
  SDL_Renderer *renderer = get_current_renderer();
  AABBCollider aabb = collider->aabb;

  // Draw the box.
  SDL_FRect rect;
  rect.x = (float)(aabb.x - aabb.w / 2);
  rect.y = (float)(aabb.y - aabb.h / 2);
  rect.h = (float)aabb.h;
  rect.w = (float)aabb.w;
  SDL_RenderFillRect(renderer, &rect);
}

/**
 * Renders an OBB collider on the screen.
 */
void render_obb_collider(Collider *collider)
{
  SDL_Renderer *renderer = get_current_renderer();
  SDL_Color color = get_collision_type_debug_color(collider->collision_type);
  OBBCollider obb = collider->obb;

  // Since SDL3 doesn't support rotated rectangle, we calculate
  // the lines to draw them for.

  // First, we get the LOCAL coordinates of the corners.
  Vector2 corners[4];
  corners[0] = (Vector2){-obb.w / 2, -obb.h / 2}; // Top left
  corners[1] = (Vector2){obb.w / 2, -obb.h / 2};  // Top right
  corners[2] = (Vector2){obb.w / 2, obb.h / 2};   // Bottom right
  corners[3] = (Vector2){-obb.w / 2, obb.h / 2};  // Bottom left

  // Rotate all based on origin and recenter.
  double sin = SDL_sin(obb.angle), cos = SDL_cos(obb.angle);
  for (int i = 0; i < 4; i++)
  {
    corners[i] = rotate_vector2_sincos(corners[i], sin, cos);
    corners[i] = add_vector2(corners[i], (Vector2){obb.x, obb.y});
  }

  // Calculate the top left to draw at.
  SDL_FRect rect;
  rect.x = (float)(obb.x - obb.w / 2);
  rect.y = (float)(obb.y - obb.h / 2);
  rect.w = (float)obb.w;
  rect.h = (float)obb.h;

  // Retrieve the float version of the color
  SDL_FColor fcolor;
  fcolor.r = color.r / 255.0f;
  fcolor.g = color.g / 255.0f;
  fcolor.b = color.b / 255.0f;
  fcolor.a = color.a / 255.0f;

  // Convert to stupid SDL vertices.
  SDL_Vertex verts[4] = {
      {{(float)corners[0].x, (float)corners[0].y}, fcolor, {0, 0}},
      {{(float)corners[1].x, (float)corners[1].y}, fcolor, {1, 0}},
      {{(float)corners[2].x, (float)corners[2].y}, fcolor, {1, 1}},
      {{(float)corners[3].x, (float)corners[3].y}, fcolor, {0, 1}},
  };

  // How to connect vertices.
  // Connect vert 0, 1, 2 to a triangle. Then vert 0, 2, 3 to a triangle.
  int indices[6] = {0, 1, 2, 0, 2, 3};

  // Render the rect
  SDL_RenderGeometry(renderer, NULL, verts, 4, indices, 6);
}

/**
 * Renders the capsule-shaped collider
 */
void render_capsule_collider(Collider *collider)
{
  SDL_Renderer *renderer = get_current_renderer();
  SDL_Color color = get_collision_type_debug_color(collider->collision_type);
  CapsuleCollider cap = collider->capsule;

  // The capsule rendered should be uhh one rectangle, and two half circles.
  // We render the rectangle first.
  SDL_FRect rect;
  double radius = cap.w / 2;
  rect.x = (float)(cap.x - cap.w / 2);
  rect.y = (float)(cap.y - cap.h / 2 + radius);
  rect.h = (float)(cap.h - radius * 2);
  rect.w = (float)cap.w;
  SDL_RenderFillRect(renderer, &rect);

  // Then we start trying to draw a half circle.
  SDL_FColor fcolor;
  fcolor.r = color.r / 255.0f;
  fcolor.g = color.g / 255.0f;
  fcolor.b = color.b / 255.0f;
  fcolor.a = color.a / 255.0f;

  render_filled_half_circle((SDL_FPoint){(float)cap.x, (float)(cap.y - cap.h / 2 + radius)}, fcolor, radius, true);
  render_filled_half_circle((SDL_FPoint){(float)cap.x, (float)(cap.y + cap.h / 2 - radius)}, fcolor, radius, false);
}

void render_collider(Collider *collider)
{
  SDL_assert(collider != NULL);

  SDL_Renderer *renderer = get_current_renderer();
  SDL_Color color = get_collision_type_debug_color(collider->collision_type);
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  switch (collider->collider_type)
  {
  case COLLIDER_TYPE_AABB:
    render_aabb_collider(collider);
    break;
  case COLLIDER_TYPE_OBB:
    render_obb_collider(collider);
    break;
  case COLLIDER_TYPE_CAPSULE:
    render_capsule_collider(collider);
    break;
  }
}
