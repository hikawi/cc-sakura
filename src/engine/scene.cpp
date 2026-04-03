#include "engine/scene.h"

#include <algorithm>

namespace ccsakura
{

void iscene::on_attach()
{
}
void iscene::on_detach()
{
}

bool iscene::on_tick(const double) noexcept
{
    return true;
}

bool iscene::on_physical_tick() noexcept
{
    return true;
}

void iscene::on_render(const sdl::irenderer &) const noexcept
{
}

// ========================
// Scene Manager
// ========================

bool scene_manager::push_back(std::unique_ptr<iscene> scene)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::push_back, std::move(scene), scene_type::dbg_none});
    return true;
}

bool scene_manager::push_front(std::unique_ptr<iscene> scene)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::push_front, std::move(scene), scene_type::dbg_none});
    return true;
}

bool scene_manager::push_before(std::unique_ptr<iscene> scene, const scene_type type)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::push_before, std::move(scene), type});
    return true;
}

bool scene_manager::push_after(std::unique_ptr<iscene> scene, const scene_type type)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::push_after, std::move(scene), type});
    return true;
}

std::unique_ptr<iscene> scene_manager::pop_front()
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::pop_front, nullptr, scene_type::dbg_none});
    return nullptr;
}

std::unique_ptr<iscene> scene_manager::pop_last()
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::pop_last, nullptr, scene_type::dbg_none});
    return nullptr;
}

std::unique_ptr<iscene> scene_manager::pop_of_type(const scene_type type)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::pop_of_type, nullptr, type});
    return nullptr;
}

bool scene_manager::emit_signal(std::unique_ptr<isignal> signal)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::emit_signal, nullptr, scene_type::dbg_none, std::move(signal)});
    return true;
}

void scene_manager::tick(const double dt)
{
    for (auto &scene : m_stack)
    {
        if (!scene->on_tick(dt))
        {
            break;
        }
    }
}

void scene_manager::physical_tick()
{
    for (auto &scene : m_stack)
    {
        if (!scene->on_physical_tick())
        {
            break;
        }
    }
}

void scene_manager::render(const sdl::irenderer &renderer) const noexcept
{
    for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
    {
        (*it)->on_render(renderer);
    }
}

void scene_manager::process_requests()
{
    std::queue<scene_request> requests_to_process;
    {
        std::lock_guard<std::mutex> lock(m_requests_mutex);
        if (m_requests.empty())
        {
            return;
        }
        requests_to_process.swap(m_requests);
    }

    while (!requests_to_process.empty())
    {
        scene_request req = std::move(requests_to_process.front());
        requests_to_process.pop();

        switch (req.type)
        {
        case scene_request_type::push_back:
            if (req.scene)
            {
                req.scene->on_attach();
                m_stack.push_back(std::move(req.scene));
            }
            break;
        case scene_request_type::push_front:
            if (req.scene)
            {
                req.scene->on_attach();
                m_stack.push_front(std::move(req.scene));
            }
            break;
        case scene_request_type::push_before:
        {
            if (req.scene)
            {
                auto it = std::find_if(m_stack.begin(), m_stack.end(),
                                       [&](const auto &s) { return s->type() == req.target_type; });
                req.scene->on_attach();
                if (it != m_stack.end())
                {
                    m_stack.insert(it, std::move(req.scene));
                }
                else
                {
                    m_stack.push_front(std::move(req.scene));
                }
            }
            break;
        }
        case scene_request_type::push_after:
        {
            if (req.scene)
            {
                auto it = std::find_if(m_stack.rbegin(), m_stack.rend(),
                                       [&](const auto &s) { return s->type() == req.target_type; });
                req.scene->on_attach();
                if (it != m_stack.rend())
                {
                    m_stack.insert(it.base(), std::move(req.scene));
                }
                else
                {
                    m_stack.push_back(std::move(req.scene));
                }
            }
            break;
        }
        case scene_request_type::pop_front:
            if (!m_stack.empty())
            {
                auto scene = std::move(m_stack.front());
                m_stack.pop_front();
                scene->on_detach();
            }
            break;
        case scene_request_type::pop_last:
            if (!m_stack.empty())
            {
                auto scene = std::move(m_stack.back());
                m_stack.pop_back();
                scene->on_detach();
            }
            break;
        case scene_request_type::pop_of_type:
        {
            auto it = std::find_if(m_stack.begin(), m_stack.end(),
                                   [&](const auto &s) { return s->type() == req.target_type; });
            if (it != m_stack.end())
            {
                auto scene = std::move(*it);
                m_stack.erase(it);
                scene->on_detach();
            }
            break;
        }
        case scene_request_type::emit_signal:
            // Handle signal emission here, e.g., pass req.signal to a signal handler
            break;
        }
    }
}

} // namespace ccsakura
