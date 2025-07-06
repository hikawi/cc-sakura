# Sakura and the Clow Cards

### A self-designed 2D platformer game made from scratch with C and SDL3

## 📖 Overview

A passion project where Kinomoto Sakura (from Cardcaptor Sakura) traverses the 2D platforming world, collecting Clow Cards to unlock new platforming abilities. The game is inspired by popular platformer games such as Super Paper Mario, Celeste, Super Mario Bros. Wonder, etc. (even though I never played the latter two). The engine is inspired by my friend, who works in Graphics and Physics Programming, specializing in raytracing and sampling lighting systems.

- **Pure C + SDL3** – I decided to go fully hands on with engine making and designing, one part as a passion project, and one as a way to learn the internal workings of popular game engines like Unity or Unreal.
- **Custom Collision System** – Supports AABB, OBB, Circle, and Capsule colliders, partially.
- **Fixed & Variable Tick System** – Smooth physics (60 FPS) + flexible animations.

## 🚀 Getting Started

Prerequisites

- SDL3: vendored in by Git Submodules.
- C Compiler (GCC or Clang): I do not recommend MSVC as MSVC is a lot more behind compared to GCC and Clang, even though we stop at C17 and don't require any fancy future features.
- CMake

Clone this repository.

```bash
git clone https://github.com/hikawi/cc-sakura
```

Recurse git submodules

## Project Structure

### Colliders

I have written simple collision checks for various primitive types of colliders. All colliders use a system where `x` and `y` refer to the collider's center, not the top left corner or something else.

Collider types:

- AABB (Axis-aligned Bounding Box): A simple rectangle. Can not be rotated. Denoted by its `x`, `y` (the center) and `w`, `h` (the full dimensions, not half extents).
- OBB (Oriented Bounding Box): A rotated AABB. Denoted by the same attributes, plus `angle`, calculated in radians. `angle=0` is pointing to the right, and positive values go counter-clockwise.
- Circle: A circular bounding box. Denoted by `x`, `y` (center) and `r` (the radius).
- Capsule: A capsule bounding box. Denoted by `p1`, `p2` (this draws the line segment of the capsule, can be rotated arbitrarily), and `r` (the thickness radius of the capsule).

`render_collider(Collider *)` is available for debugging purposes, provided by `collider_renderer.h`.

| Collider A | Collider B | Implemented? |        Reuse?         |
| :--------: | :--------: | :----------: | :-------------------: |
|    AABB    |    AABB    |    ✅ Yes    |     ⚡ Optimized      |
|    AABB    |    OBB     |    ✅ Yes    |   Reuse OBB vs OBB    |
|    AABB    |   Circle   |    ✅ Yes    |     ⚡ Optimized      |
|    AABB    |  Capsule   |    ✅ Yes    |     ⚡ Optimized      |
|    OBB     |    OBB     |    ✅ Yes    |     ⚡ Optimized      |
|    OBB     |   Circle   |    ✅ Yes    | Reuse AABB vs Circle  |
|    OBB     |  Capsule   |    ✅ Yes    | Reuse AABB vs Capsule |
|   Circle   |   Circle   |    ✅ Yes    |     ⚡ Optimized      |
|   Circle   |  Capsule   |    ✅ Yes    |     ⚡ Optimized      |
|  Capsule   |  Capsule   |    ❌ No     |                       |

### Tick System

There are expected to be two types of functions of handling for any dynamic actors (moving platforms, sensors, enemies, the player), a variable update and a fixed update. A variable update runs every single frame, meant to be use for animations, particles or UI elements. A fixed update tries to run 60 times a second, meant to be used for physical updates and collision checks.

For each dynamic actor, please create a function that conforms to a simple convention `update_actor_name(AppState *, double)` and `fixed_update_actor_name(AppState *)`.

### Collisions

Since this is a 2D platformer, collisions are optimized using a quadtree. A quadtree is a hierarchical spatial partitioning structure where each node represents a rectangular region that can be subdivided into 4 quadrants: top-left (TL), top-right (TR), bottom-left (BL), and bottom-right (BR).

On each physics tick, dynamic actors query the quadtree to retrieve only nearby colliders within their spatial region, avoiding unnecessary checks against unrelated objects.
