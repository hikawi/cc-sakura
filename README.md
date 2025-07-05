# Sakura and the Clow Cards

### A self-designed 2D platformer game made from scratch with C and SDL3

## Overview

For learning purposes, this is a very simple and primitive 2D platformer game, made with SDL3, and maybe later on with some libraries to help with the game engine. The game revolves around a magical girl named Kinomoto Sakura, who is on a mission to recollect all of the Clow Cards. For each Clow Card she collects, she gets a new ability for platforming.

## Project Structure

### Colliders

I have written simple collision checks for various primitive types of colliders. All colliders use a system where `x` and `y` refer to the collider's center, not the top left corner or something else.

Collider types:

- AABB (Axis-aligned Bounding Box): A simple rectangle. Can not be rotated. Denoted by its `x`, `y` (the center) and `w`, `h` (the full dimensions, not half extents).
- OBB (Oriented Bounding Box): A rotated AABB. Denoted by the same attributes, plus `angle`, calculated in radians. `angle=0` is pointing to the right, and positive values go counter-clockwise.
- Circle: A circular bounding box. Denoted by `x`, `y` (center) and `r` (the radius).
- Capsule: A capsule bounding box. Denoted by `p1`, `p2` (this draws the line segment of the capsule, can be rotated arbitrarily), and `r` (the thickness radius of the capsule).

**Currently, OBB is not implemented.**

`render_collider(Collider *)` is available for debugging purposes, provided by `collider_renderer.h`.

### Tick System

There are expected to be two types of functions of handling for any dynamic actors (moving platforms, sensors, enemies, the player), a variable update and a fixed update. A variable update runs every single frame, meant to be use for animations, particles or UI elements. A fixed update tries to run 60 times a second, meant to be used for physical updates and collision checks.

For each dynamic actor, please create a function that conforms to a simple convention `update_actor_name(AppState *, double)` and `fixed_update_actor_name(AppState *)`.

### Collisions

Since this is a 2D platformer, collisions are optimized using a quadtree. A quadtree is a hierarchical spatial partitioning structure where each node represents a rectangular region that can be subdivided into 4 quadrants: top-left (TL), top-right (TR), bottom-left (BL), and bottom-right (BR).

On each physics tick, dynamic actors query the quadtree to retrieve only nearby colliders within their spatial region, avoiding unnecessary checks against unrelated objects.
