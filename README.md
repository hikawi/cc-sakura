# Sakura and the Clow Cards

### A self-designed 2D platformer game made from scratch with C and SDL3

## Overview

For learning purposes, this is a very simple and primitive 2D platformer game, made with SDL3, and maybe later on with some libraries to help with the game engine. The game revolves around a magical girl named Kinomoto Sakura, who is on a mission to recollect all of the Clow Cards. For each Clow Card she collects, she gets a new ability for platforming.

## Project Structure

### Colliders & Collisions

I have written simple collision checks for various primitive types of colliders. All colliders use a system where `x` and `y` refer to the collider's center, not the top left corner or something else.

Collider types:

- AABB (Axis-aligned Bounding Box): A simple rectangle. Can not be rotated. Denoted by its `x`, `y` (the center) and `w`, `h` (the full dimensions, not half extents).
- OBB (Oriented Bounding Box): A rotated AABB. Denoted by the same attributes, plus `angle`, calculated in radians. `angle=0` is pointing to the right, and positive values go counter-clockwise.
- Circle: A circular bounding box. Denoted by `x`, `y` (center) and `r` (the radius).
- Capsule: A capsule bounding box. Can not be rotated, made with an AABB and two circle colliders. Denoted by `x`, `y` (the center), and `w`, `h` (the full dimensions).

**Currently, OBB is not implemented.**
