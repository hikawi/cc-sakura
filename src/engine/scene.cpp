#include "engine/scene.h"

namespace ccsakura
{

void iscene::on_attach()
{
}

void iscene::on_detach()
{
}

void iscene::on_enter()
{
}

void iscene::on_exit()
{
}

void iscene::on_tick(const double) noexcept
{
}

void iscene::on_physical_tick() noexcept
{
}

bool iscene::on_signal(isignal & /*signal*/) noexcept
{
    return false;
}

void iscene::on_render(const sdl::irenderer & /*renderer*/) const noexcept
{
}

bool iscene::is_modal() const noexcept
{
    return false;
}

bool iscene::is_opaque() const noexcept
{
    return false;
}

} // namespace ccsakura
