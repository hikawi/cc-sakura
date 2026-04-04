#pragma once

#include "engine/collision.h"
#include "engine/scene.h"
#include "sdl/sdl_render.h"

#include <memory>
#include <vector>

namespace ccsakura::scenes
{

/**
 * Scene to test collider rendering.
 */
class dbg_colliders : public ccsakura::iscene
{
  public:
    dbg_colliders();
    ~dbg_colliders() = default;
    scene_type type() const noexcept override;
    void on_attach(scene_context &ctx) noexcept override;
    void on_detach(scene_context &ctx) noexcept override;
    bool on_tick(scene_context &ctx, const double dt) noexcept override;
    void on_render(const sdl::irenderer &renderer) const noexcept override;

  private:
    std::vector<std::unique_ptr<collider>> m_colliders;
    aabb_collider *m_aabb;
    obb_collider *m_obb;
    circle_collider *m_circle;
    capsule_collider *m_capsule;

    double m_time = 0;
};

} // namespace ccsakura::scenes
