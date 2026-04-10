#pragma once

#include "engine/scene.h"

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

  private:
    static constexpr uint32_t ENTITY_SCORE = 3;

    vec2d m_velocity;
    uint64_t m_score = 0;
};

} // namespace ccsakura::scenes
