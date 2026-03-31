#pragma once

#include "engine/scene.h"
#include "sdl/sdl_render.h"

namespace ccsakura::scenes
{

/**
 * Scene to display a simple FPS counter on the top right to test the scene-layering system.
 */
class dbg_empty : public ccsakura::iscene
{
  public:
    dbg_empty(sdl::fcolor color);
    ~dbg_empty() = default;
    scene_type type() const noexcept override;
    void on_render(const sdl::irenderer &renderer) const noexcept override;

  private:
    sdl::fcolor m_color;
};

} // namespace ccsakura::scenes
