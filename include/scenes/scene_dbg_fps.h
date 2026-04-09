#pragma once

#include "engine/scene.h"
#include "engine/text.h"

namespace ccsakura::scenes
{

/**
 * Scene to display a simple FPS counter on the top right to test the scene-layering system.
 */
class dbg_fps : public ccsakura::iscene
{
  public:
    dbg_fps();
    ~dbg_fps() = default;
    scene_type type() const noexcept override;
    void on_attach(scene_context &ctx) noexcept override;
    void on_detach(scene_context &ctx) noexcept override;
    bool on_tick(scene_context &ctx, const double dt) noexcept override;

  private:
    text m_fps_text;

    double m_accumulator = 0;
    uint32_t m_frame_count = 0;
    uint32_t m_fps = 0;
};

} // namespace ccsakura::scenes
