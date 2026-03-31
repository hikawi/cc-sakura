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

// ======================================

std::unique_ptr<iscene> scene_manager::push_front(std::unique_ptr<iscene> scene) noexcept
{
    if (!scene)
    {
        return nullptr;
    }

    scene->on_attach();
    m_scenes.push_front(std::move(scene));

    return nullptr;
}

std::unique_ptr<iscene> scene_manager::push_back(std::unique_ptr<iscene> scene) noexcept
{
    if (!scene)
    {
        return nullptr;
    }

    scene->on_attach();
    m_scenes.push_back(std::move(scene));

    return nullptr;
}

void scene_manager::tick(const double dt) const noexcept
{
    for (auto it = m_scenes.cbegin(); it != m_scenes.cend(); it++)
    {
        iscene &scene = **it;
        scene.on_tick(dt);

        if (scene.is_modal())
        {
            break;
        }
    }
}

void scene_manager::physical_tick() const noexcept
{
    for (auto it = m_scenes.cbegin(); it != m_scenes.cend(); it++)
    {
        iscene &scene = **it;
        scene.on_physical_tick();

        if (scene.is_modal())
        {
            break;
        }
    }
}

void scene_manager::render(const sdl::irenderer &renderer) const noexcept
{
    for (auto it = m_scenes.rbegin(); it != m_scenes.rend(); it++)
    {
        iscene &scene = **it;
        scene.on_render(renderer);
    }
}

} // namespace ccsakura
