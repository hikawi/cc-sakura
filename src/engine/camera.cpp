#include "engine/camera.h"

namespace ccsakura
{

sdl::fpoint world_to_screen(const vec2d &world, const camera2d &cam) noexcept
{
    const vec2d screen = (world - cam.position) * cam.zoom + cam.viewport * 0.5;
    return screen.to_fpoint();
}

sdl::frect world_to_screen(const sdl::frect &world, const camera2d &cam) noexcept
{
    const sdl::fpoint top_left = world_to_screen(vec2d{double(world.x), double(world.y)}, cam);
    return {top_left.x, top_left.y, world.w * static_cast<float>(cam.zoom), world.h * static_cast<float>(cam.zoom)};
}

} // namespace ccsakura
