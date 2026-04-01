#include "engine/scene.h"

namespace ccsakura
{

void iscene::on_attach()
{
}

void iscene::on_detach()
{
}

bool iscene::on_tick(const double /*dt*/) noexcept
{
    return true;
}

bool iscene::on_physical_tick() noexcept
{
    return true;
}

bool iscene::on_signal(isignal & /*signal*/) noexcept
{
    return true;
}

void iscene::on_render(const sdl::irenderer & /*renderer*/) const noexcept
{
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

std::unique_ptr<iscene> scene_manager::pop_front() noexcept
{
    if (m_scenes.empty())
    {
        return nullptr;
    }

    std::unique_ptr<iscene> scene = std::move(m_scenes.front());
    scene->on_detach();
    m_scenes.pop_front();

    return scene;
}

std::unique_ptr<iscene> scene_manager::pop_back() noexcept
{
    if (m_scenes.empty())
    {
        return nullptr;
    }

    std::unique_ptr<iscene> scene = std::move(m_scenes.back());
    scene->on_detach();
    m_scenes.pop_back();
    return scene;
}

void scene_manager::tick(const double dt) const noexcept
{
    for (auto it = m_scenes.cbegin(); it != m_scenes.cend(); it++)
    {
        iscene &scene = **it;
        if (!scene.on_tick(dt))
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
        if (!scene.on_physical_tick())
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
