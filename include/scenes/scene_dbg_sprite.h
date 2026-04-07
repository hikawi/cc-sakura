#pragma once

#include "engine/collision.h"
#include "engine/entity.h"
#include "engine/scene.h"
#include "engine/text.h"
#include "sdl/sdl_render.h"

#include <memory>

namespace ccsakura::scenes
{

/**
 * Scene to test sprite animation rendering.
 */
class dbg_sprite : public ccsakura::iscene
{
  public:
    dbg_sprite();
    ~dbg_sprite() = default;
    scene_type type() const noexcept override;
    void on_attach(scene_context &ctx) noexcept override;
    void on_detach(scene_context &ctx) noexcept override;
    bool on_physical_tick(scene_context &ctx) noexcept override;
    bool on_tick(scene_context &ctx, const double dt) noexcept override;
    void on_render(const sdl::irenderer &renderer) const noexcept override;

  private:
    std::unique_ptr<entity> m_entity;
    std::unique_ptr<entity> m_key_entity;
    circle_collider m_ball_collider;
    aabb_collider m_key_collider;
    vec2d m_velocity;
    text m_score_text;
    uint64_t m_score;
};

} // namespace ccsakura::scenes
