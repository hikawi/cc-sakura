/**
 * \file scenes/scene_id.h
 *
 * Provides a list of numerical IDs for scenes.
 */

#pragma once

namespace ccsakura
{

enum class scene_type
{
    dbg_none = 1000000000,
    dbg_fps = 1000000001,
    dbg_empty = 1000000002,
    dbg_colliders = 1000000003,
    dbg_sprite = 1000000004,
    dbg_physics = 1000000005,
};

} // namespace ccsakura
