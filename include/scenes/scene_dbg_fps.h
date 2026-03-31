#pragma once

#include "engine/scene.h"
#include "engine/text.h"
#include "sdl/sdl_render.h"

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
    void on_tick(const double dt) noexcept override;
    void on_render(const sdl::irenderer &renderer) const noexcept override;

  private:
    text m_fps_text;

    double m_accumulator = 0;
    uint32_t m_frame_count = 0;
    uint32_t m_fps = 0;
};

} // namespace ccsakura::scenes
