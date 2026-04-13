/**
 * \file scenes/scene_id.h
 *
 * Provides scene category types and instance handles.
 */

#pragma once

#include <cstdint>

namespace ccsakura
{

/**
 * \brief Categorises a scene by its role in the stack.
 *
 * Used for positional insertion (push_before, push_after) and category-level
 * removal (pop_of_type). Not an identity — multiple scenes may share the same
 * type. For instance targeting use scene_handle instead.
 */
enum class scene_type
{
    none = 0, ///< Sentinel / default.
    debug,    ///< Any development or debug scene.
    // Future: hud, map, battle, overlay, ...
};

/**
 * \brief Unique instance identifier assigned by the scene manager when a scene is pushed.
 *
 * Returned by push_back, push_front, push_before, push_after, and start_transition.
 * Use this handle to target a specific scene instance in start_transition or pop().
 */
using scene_handle = uint64_t;

/// Sentinel value indicating an unassigned or invalid handle.
static constexpr scene_handle invalid_scene_handle = 0;

} // namespace ccsakura
