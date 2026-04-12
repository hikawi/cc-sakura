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
    dbg_fps,
    dbg_empty,
    dbg_sprite,
    dbg_physics,
};

} // namespace ccsakura
