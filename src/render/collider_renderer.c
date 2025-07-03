#include "render/collider_renderer.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "engine/collision.h"
#include "misc/vector.h"
#include "render/renderer.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Renders a filled half circle, using a center and a radius.
 */
void render_filled_half_circle(SDL_FPoint center, SDL_FColor fcolor, double radius, double theta)
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

  // Rotate each vertices by the center.
  double sin_theta = SDL_sin(theta), cos_theta = SDL_cos(theta);
  for (int i = 0; i < (int)segments + 2; i++)
  {
    // Translate to local coordinates, rotate, and translate back.
    Vector2 local;
    local.x = (double)(vertices[i].position.x - center.x);
    local.y = (double)(vertices[i].position.y - center.y);
    local = rotate_vector2_sincos(local, sin_theta, cos_theta);

    vertices[i].position.x = (float)local.x + center.x;
    vertices[i].position.y = (float)local.y + center.y;
  }

  // Render
  SDL_Renderer *renderer = get_current_renderer();
  SDL_RenderGeometry(renderer, NULL, vertices, (int)segments + 2, triangles, (int)segments * 3);

  // Clean up
  free(vertices);
  free(triangles);
}

/**
 * Renders a rotated rectangle at a color, defined by 4 points.
 *
 * Points must be in local coordinates.
 *
 * This always assumes points is a proper array with 4 points, basically being
 * top left, top right, bottom right, bottom left.
 */
void render_rotated_rectangle(SDL_Color color, Vector2 points[4], Vector2 origin, double angle)
{
  SDL_Renderer *renderer = get_current_renderer();

  // Rotate all based on origin and recenter.
  double sin = SDL_sin(angle), cos = SDL_cos(angle);
  for (int i = 0; i < 4; i++)
  {
    points[i] = rotate_vector2_sincos(points[i], sin, cos);
    points[i] = add_vector2(points[i], origin);
  }

  // Retrieve the float version of the color
  SDL_FColor fcolor;
  fcolor.r = color.r / 255.0f;
  fcolor.g = color.g / 255.0f;
  fcolor.b = color.b / 255.0f;
  fcolor.a = color.a / 255.0f;

  // Convert to stupid SDL vertices.
  SDL_Vertex verts[4] = {
      {{(float)points[0].x, (float)points[0].y}, fcolor, {0, 0}},
      {{(float)points[1].x, (float)points[1].y}, fcolor, {0, 0}},
      {{(float)points[2].x, (float)points[2].y}, fcolor, {0, 0}},
      {{(float)points[3].x, (float)points[3].y}, fcolor, {0, 0}},
  };

  // How to connect vertices.
  // Connect vert 0, 1, 2 to a triangle. Then vert 0, 2, 3 to a triangle.
  int indices[6] = {0, 1, 2, 0, 2, 3};

  // Render the rect
  SDL_RenderGeometry(renderer, NULL, verts, 4, indices, 6);
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
  SDL_Color color = get_collision_type_debug_color(collider->collision_type);
  OBBCollider obb = collider->obb;
  render_rotated_rectangle(color,
                           (Vector2[4]){{-obb.w / 2, -obb.h / 2}, {obb.w / 2, -obb.h / 2}, {obb.w / 2, obb.h / 2}, {-obb.w / 2, obb.h / 2}},
                           (Vector2){obb.x, obb.y}, obb.angle);
}

/**
 * Renders the capsule-shaped collider
 */
void render_capsule_collider(Collider *collider)
{
  SDL_Color color = get_collision_type_debug_color(collider->collision_type);
  CapsuleCollider cap = collider->capsule;

  Vector2 dir;
  dir.x = cap.p2.x - cap.p1.x;
  dir.y = cap.p2.y - cap.p1.y;
  dir = normalize_vector2(dir);

  // Calculate the normal vector for the capsule.
  // First, we calculate the direction of the vector, then flip it 90 degrees.
  Vector2 normal;
  normal = rotate_vector2(dir, -M_PI_2);
  normal = normalize_vector2(normal);

  // Get the capsule's origin.
  Vector2 origin;
  origin.x = (cap.p2.x + cap.p1.x) / 2;
  origin.y = (cap.p2.y + cap.p1.y) / 2;

  // Calculate the corners of the rectangle to draw.
  Vector2 corners[4];
  corners[0] = add_vector2(cap.p1, scale_vector2(normal, cap.r));
  corners[0] = add_vector2(corners[0], scale_vector2(dir, cap.r));
  corners[0] = add_vector2(corners[0], scale_vector2(origin, -1));

  corners[1] = add_vector2(cap.p1, scale_vector2(normal, -cap.r));
  corners[1] = add_vector2(corners[1], scale_vector2(dir, cap.r));
  corners[1] = add_vector2(corners[1], scale_vector2(origin, -1));

  corners[2] = add_vector2(cap.p2, scale_vector2(normal, -cap.r));
  corners[2] = add_vector2(corners[2], scale_vector2(dir, -cap.r));
  corners[2] = add_vector2(corners[2], scale_vector2(origin, -1));

  corners[3] = add_vector2(cap.p2, scale_vector2(normal, cap.r));
  corners[3] = add_vector2(corners[3], scale_vector2(dir, -cap.r));
  corners[3] = add_vector2(corners[3], scale_vector2(origin, -1));
  render_rotated_rectangle(color, corners, origin, get_rotation(normal));

  // Then we start trying to draw a half circle.
  SDL_FColor fcolor;
  fcolor.r = color.r / 255.0f;
  fcolor.g = color.g / 255.0f;
  fcolor.b = color.b / 255.0f;
  fcolor.a = color.a / 255.0f;

  // Calculate the center for two semi circles.
  SDL_FPoint c1;
  c1.x = (float)(cap.p1.x + dir.x * cap.r);
  c1.y = (float)(cap.p1.y + dir.y * cap.r);

  SDL_FPoint c2;
  c2.x = (float)(cap.p2.x - dir.x * cap.r);
  c2.y = (float)(cap.p2.y - dir.y * cap.r);

  render_filled_half_circle(c1, fcolor, cap.r, get_rotation(normal) + M_PI);
  render_filled_half_circle(c2, fcolor, cap.r, get_rotation(normal));
}

void render_circle_collider(Collider *collider)
{
  SDL_Color color = get_collision_type_debug_color(collider->collision_type);
  CircleCollider circ = collider->circle;

  // Then we start trying to draw a half circle.
  SDL_FColor fcolor;
  fcolor.r = color.r / 255.0f;
  fcolor.g = color.g / 255.0f;
  fcolor.b = color.b / 255.0f;
  fcolor.a = color.a / 255.0f;

  render_filled_half_circle((SDL_FPoint){(float)circ.x, (float)circ.y}, fcolor, circ.r, 0);
  render_filled_half_circle((SDL_FPoint){(float)circ.x, (float)circ.y}, fcolor, circ.r, -M_PI);
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
  case COLLIDER_TYPE_CIRCLE:
    render_circle_collider(collider);
    break;
  }
}
