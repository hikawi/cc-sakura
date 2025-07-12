<div align="center">

# Sakura and the Clow Cards

A self-designed, built from the ground up 2D platformer game, using C and SDL3

![GitHub top language](https://img.shields.io/github/languages/top/hikawi/cc-sakura?logo=c&label=C)
![GitHub branch check runs](https://img.shields.io/github/check-runs/hikawi/cc-sakura/main)
![GitHub Tag](https://img.shields.io/github/v/tag/hikawi/cc-sakura)

</div>

## Table of Contents

- [Sakura and the Clow Cards](#sakura-and-the-clow-cards)
  - [Overview](#overview)
  - [Getting Started](#getting-started)
  - [Coding Conventions](#coding-conventions)
  - [Progress](#progress)
    - [Week 1](#week-1)
    - [Week 2](#week-2)

## Overview

A passion project where Kinomoto Sakura (from Cardcaptor Sakura) traverses the 2D platforming world, collecting Clow Cards to unlock new platforming abilities. The game is inspired by popular platformer games such as Super Paper Mario, Celeste, Super Mario Bros. Wonder, etc. (even though I never played the latter two). The engine is inspired by my friend, who works in Graphics and Physics Programming, specializing in raytracing and sampling lighting systems.

- **Pure C + SDL3** – I decided to go fully hands on with engine making and designing, one part as a passion project, and one as a way to learn the internal workings of popular game engines like Unity or Unreal.
- **Custom Collision System** – Supports AABB, OBB, Circle, and Capsule colliders, partially.
- **Fixed & Variable Tick System** – Smooth physics (60 FPS) + flexible animations.

## Getting Started

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

## Coding Conventions

To put it simply:

- All variables and functions names are `snake_case`, including local variables or struct properties.
- Struct names are `PascalCase`.
- Global constants are `SCREAMING_SNAKE_CASE`, including defines.
- Global variables should still be in `snake_case`.
- Make a comment block above each header file, to explain what this header file is responsible for.
- Use `#pragma once` instead of `#ifndef #define`.
- Functions are truly in the global namespace, make sure your function names reflect that specific function's actions, and be as detailed as possible without making the name too long. For example, prefer `normalize_vector2d(Vector2D)` over `normalize(Vector2D)`.

## Progress

This is to show videos, or screenshots of what is being done as days passed.

### Week 1

<details>
    <summary>Day 3 (Sprite Rendering and Movement)</summary>

https://github.com/user-attachments/assets/1c4963c5-ea70-4501-96eb-e63d0a730627

</details>

<details>
    <summary>Day 5 (Simple Collisions)</summary>

https://github.com/user-attachments/assets/6912306c-b8cf-4e80-ab8c-f68f80f74a85

</details>

<details>
    <summary>Day 7 (Gravity)</summary>

https://github.com/user-attachments/assets/6bc00f98-53f8-477e-8300-f21fcaf703d6

</details>

### Week 2

<details>
    <summary>Day 9 (Map Load and Autotile)</summary>

<img width="1600" alt="Map loading from file" src="https://github.com/user-attachments/assets/7db2e674-697f-471b-ba8d-70075ed9586b" />

</details>

<details>
    <summary>Day 11 (Scenes, Scene Manager & Scene Transitions)</summary>

https://github.com/user-attachments/assets/402e0f31-4395-4e4f-8551-3bb3c04623b8

</details>
