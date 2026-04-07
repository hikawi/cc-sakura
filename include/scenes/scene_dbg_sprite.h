#pragma once

#include "engine/scene.h"
#include "engine/text.h"
#include "sdl/sdl_render.h"

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
    vec2d m_velocity;
    text m_score_text;
    uint64_t m_score;
};

} // namespace ccsakura::scenes
