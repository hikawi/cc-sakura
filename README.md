<div align="center">

# Sakura and the Clow Cards

A self-designed, built from the ground up 2D platformer game, using C and SDL3

</div>

## Overview

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
git clone --recurse-submodules https://github.com/hikawi/cc-sakura
```

Pre-build the project

```bash
cmake -S . -B build && cmake --build build
```

Run the binary, you can run it with CMake:

```bash
cmake --build build --target run
```

Or you can run it directly (via double-clicking the binary file or through the command line), on the binary file generated in `build/bin`.

## 🖊️ Coding Conventions

To put it simply:

- All variables and functions names are `snake_case`, including local variables or struct properties.
- Struct names are `PascalCase`.
- Global constants are `SCREAMING_SNAKE_CASE`, including defines.
- Global variables should still be in `snake_case`.
- Make a comment block above each header file, to explain what this header file is responsible for.
- Use `#pragma once` instead of `#ifndef #define`.
- Functions are truly in the global namespace, make sure your function names reflect that specific function's actions, and be as detailed as possible without making the name too long. For example, prefer `normalize_vector2d(Vector2D)` over `normalize(Vector2D)`.
