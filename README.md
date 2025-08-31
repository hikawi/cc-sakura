<div align="center">

# Sakura and the Clow Cards

A self-designed, built from the ground up 2D platformer game, using C and SDL3

![GitHub repo size](https://img.shields.io/github/repo-size/hikawi/cc-sakura)
![GitHub branch check runs](https://img.shields.io/github/check-runs/hikawi/cc-sakura/main)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/hikawi/cc-sakura/docs.yml?label=docs)
![](https://tokei.rs/b1/github/hikawi//cc-sakura?category=code)
[![CodeFactor](https://www.codefactor.io/repository/github/hikawi/cc-sakura/badge)](https://www.codefactor.io/repository/github/hikawi/cc-sakura)

</div>

## Table of Contents

- [Sakura and the Clow Cards](#sakura-and-the-clow-cards)
  - [Overview](#overview)
  - [Getting Started](#getting-started)
  - [Coding Conventions](#coding-conventions)
  - [Progress](#progress)
    - [Week 1](#week-1)
    - [Week 2](#week-2)
    - [Week 3](#week-3)

## Overview

A passion project where Kinomoto Sakura (from Cardcaptor Sakura) traverses the 2D platforming world, collecting Clow Cards to unlock new platforming abilities. The game is inspired by popular platformer games such as Super Paper Mario, Celeste, Super Mario Bros. Wonder, etc. (even though I never played the latter two). The engine is inspired by my friend, who works in Graphics and Physics Programming, specializing in raytracing and sampling lighting systems.

- **C/C++ + SDL3** – I decided to go fully hands on with engine making and designing, one part as a passion project, and one as a way to learn the internal workings of popular game engines like Unity or Unreal.
- **Custom Collision System** – Supports AABB, OBB, Circle, and Capsule colliders, partially.
- **Fixed & Variable Tick System** – Smooth physics (60 FPS) + flexible animations.

Public Wiki is available [here](https://cc-sakura.luny.dev/).

## Getting Started

Prerequisites

- SDL3: vendored in by Git Submodules.
- C Compiler (GCC or Clang): I do not recommend MSVC as MSVC is a lot more behind compared to GCC and Clang (unless you're on Windows). Make sure your compiler supports C++23 features.
- CMake

Clone this repository.

```bash
git clone https://github.com/hikawi/cc-sakura
```

Pre-build the project

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="path/to/vcpkg/buildsystems/vcpkg.cmake" -DCMAKE_CXX_COMPILER="path/to/cxx/compiler"
cmake --build build -j
```

You can run it directly (via double-clicking the binary file or through the command line), on the binary file generated in `build/bin`.

## Progress

This is to show videos, or screenshots of what is being done as days passed.

### Month 1

<details>
    <summary>Day 3 - July 2, 2025 (Sprite Rendering and Movement)</summary>

<https://github.com/user-attachments/assets/1c4963c5-ea70-4501-96eb-e63d0a730627>

</details>

<details>
    <summary>Day 5 - July 4, 2025 (Simple Collisions)</summary>

<https://github.com/user-attachments/assets/6912306c-b8cf-4e80-ab8c-f68f80f74a85>

</details>

<details>
    <summary>Day 7 - July 6, 2025 (Gravity)</summary>

<https://github.com/user-attachments/assets/6bc00f98-53f8-477e-8300-f21fcaf703d6>

</details>

<details>
    <summary>Day 10 - July 9, 2025 (Map Load and Autotile)</summary>

<img width="1600" alt="Map loading from file" src="https://github.com/user-attachments/assets/7db2e674-697f-471b-ba8d-70075ed9586b" />

</details>

<details>
    <summary>Day 12 - July 11, 2025 (Scenes, Scene Manager & Scene Transitions)</summary>

<https://github.com/user-attachments/assets/402e0f31-4395-4e4f-8551-3bb3c04623b8>

</details>

<details>
    <summary>Day 14 - July 13, 2025 (Revamped scenes manager)</summary>

<https://github.com/user-attachments/assets/89be68a7-c340-4a58-99b9-2672e3a3a4fe>

</details>

<details>
    <summary>Day 18 - July 17, 2025 (Signals system in scenes)</summary>

<https://github.com/user-attachments/assets/881e791a-f1d1-45a7-8bc7-eb9bb9d3c4dd>

</details>

<details>
    <summary>Day 21 - July 20, 2025 (Centralized Loading Screen)</summary>

<https://github.com/user-attachments/assets/ad2798eb-8ad4-4717-bd43-390a95b9a4e4>

</details>

<details>
    <summary>Day 24 - July 23, 2025 (Doxygen Documentation)</summary>

<https://cc-sakura.luny.dev/>

</details>
