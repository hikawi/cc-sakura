#pragma once

#include "engine/entity.h"
#include "engine/scene.h"
#include "engine/signal.h"
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
    void on_key_event(signals::key &e);

    std::unique_ptr<entity> m_entity;
    vec2d m_velocity;
    uint64_t m_key_callback;
    struct
    {
        bool up;
        bool left;
        bool down;
        bool right;
    } m_keys;
};

} // namespace ccsakura::scenes
