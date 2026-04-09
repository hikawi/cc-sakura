#pragma once

#include "engine/scene.h"
#include "sdl/sdl_render.h"

namespace ccsakura::scenes
{

/**
 * Scene to display a simple background color to test the scene-layering system.
 */
class dbg_empty : public ccsakura::iscene
{
  public:
    dbg_empty(sdl::fcolor color);
    ~dbg_empty() = default;
    scene_type type() const noexcept override;
    void on_attach(scene_context &ctx) noexcept override;
    void on_detach(scene_context &ctx) noexcept override;
    bool on_tick(scene_context &ctx, const double dt) noexcept override;

  private:
    sdl::fcolor m_color;
    sdl::fpoint m_point;

    uint64_t m_mouse_event_callback;

    void on_mouse_event(signals::mouse &event);
};

} // namespace ccsakura::scenes
