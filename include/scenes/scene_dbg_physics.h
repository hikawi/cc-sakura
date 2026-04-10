#pragma once

#include "engine/scene.h"
#include "engine/text.h"

namespace ccsakura::scenes
{

/**
 * Scene to test basic physics collision and resolution.
 */
class dbg_physics : public ccsakura::iscene
{
  public:
    dbg_physics();
    ~dbg_physics() = default;
    scene_type type() const noexcept override;
    void on_attach(scene_context &ctx) noexcept override;
    void on_detach(scene_context &ctx) noexcept override;
    bool on_physical_tick(scene_context &ctx) noexcept override;
    bool on_tick(scene_context &ctx, const double dt) noexcept override;

  private:
    void resolve_player(uint32_t a, uint32_t b, const collision &col) noexcept;

    vec2d m_velocity;
    text m_info_text;
    bool m_grounded = false;
    double m_box_dir = 1.0;
    uint64_t m_ticks = 0;
};

} // namespace ccsakura::scenes
