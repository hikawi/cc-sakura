#include "engine/scene_manager.h"

#include "engine/component/collider.h"
#include "engine/render_helper.h"
#include "engine/system/collision.h"
#include "engine/system/render.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_surface.h"

#include <algorithm>
#include <iterator>
#include <unordered_set>
#include <utility>

namespace ccsakura
{

scene_handle scene_manager::push_back(std::unique_ptr<iscene> scene)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    scene_handle h = ++m_scene_handle_counter;
    m_requests.push({scene_request_type::push_back, std::move(scene), scene_type::none, nullptr, {}, h});
    return h;
}

scene_handle scene_manager::push_front(std::unique_ptr<iscene> scene)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    scene_handle h = ++m_scene_handle_counter;
    m_requests.push({scene_request_type::push_front, std::move(scene), scene_type::none, nullptr, {}, h});
    return h;
}

scene_handle scene_manager::push_before(std::unique_ptr<iscene> scene, const scene_type type)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    scene_handle h = ++m_scene_handle_counter;
    m_requests.push({scene_request_type::push_before, std::move(scene), type, nullptr, {}, h});
    return h;
}

scene_handle scene_manager::push_after(std::unique_ptr<iscene> scene, const scene_type type)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    scene_handle h = ++m_scene_handle_counter;
    m_requests.push({scene_request_type::push_after, std::move(scene), type, nullptr, {}, h});
    return h;
}

void scene_manager::pop_front()
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::pop_front});
}

void scene_manager::pop_last()
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::pop_last});
}

void scene_manager::pop_of_type(const scene_type type)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::pop_of_type, nullptr, type});
}

void scene_manager::pop(scene_handle handle)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::pop, nullptr, scene_type::none, nullptr, {}, invalid_scene_handle, handle});
}

scene_handle scene_manager::start_transition(std::unique_ptr<iscene> to_scene, scene_handle from,
                                             scene_transition transition)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    scene_handle to_handle = ++m_scene_handle_counter;
    m_requests.push({scene_request_type::start_transition, std::move(to_scene), scene_type::none, nullptr, transition,
                     to_handle, from});
    return to_handle;
}

bool scene_manager::emit_signal(std::unique_ptr<isignal> signal)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::emit_signal, nullptr, scene_type::none, std::move(signal)});
    return true;
}

void scene_manager::tick(const double dt)
{
    for (auto &t : m_transitions)
        t.elapsed += dt;

    auto it = m_transitions.begin();
    while (it != m_transitions.end())
    {
        if (it->elapsed < it->transition.duration)
        {
            ++it;
            continue;
        }
        iscene *from = it->from_scene;
        iscene *to = it->to_scene;
        it = m_transitions.erase(it); // erase before lifecycle calls to prevent re-entry

        auto from_it = std::find_if(m_stack.begin(), m_stack.end(), [from](const auto &s) { return s.get() == from; });
        if (from_it != m_stack.end())
        {
            auto scene = std::move(*from_it);
            m_stack.erase(from_it);
            m_render_targets.erase(scene.get());
            scene->on_detach(*this);
        }

        auto to_it = std::find_if(m_stack.begin(), m_stack.end(), [to](const auto &s) { return s.get() == to; });
        if (to_it != m_stack.end())
            to->on_start(*this);
    }

    for (auto &scene : m_stack)
    {
        if (!scene->on_tick(*this, dt))
        {
            break;
        }
    }
}

void scene_manager::physical_tick()
{
    for (auto &scene : m_stack)
    {
        if (!scene->on_physical_tick(*this))
        {
            break;
        }
    }
}

void scene_manager::collision_tick()
{
    for (auto &scene : m_stack)
    {
        std::vector<std::pair<uint32_t, const components::collider *>> collidables;

        for (const auto &[id, ent] : scene->entities())
        {
            if (const auto *hb = ent->get_component<components::collider>())
                collidables.push_back({id, hb});
        }

        // Narrow phase — broad phase can be inserted here in the future
        for (size_t i = 0; i < collidables.size(); ++i)
        {
            for (size_t j = i + 1; j < collidables.size(); ++j)
            {
                // Guarantee a < b ordering before calling on_collide
                const auto [id_i, coll_i] = collidables[i];
                const auto [id_j, coll_j] = collidables[j];

                auto [id_a, id_b] = std::minmax(id_i, id_j);
                const components::collider *coll_a = (id_a == id_i) ? coll_i : coll_j;
                const components::collider *coll_b = (id_a == id_i) ? coll_j : coll_i;

                const auto col = system::check_collisions(*coll_a, *coll_b);
                if (col.is_colliding)
                {
                    scene->on_collide(id_a, id_b, col);
                }
            }
        }
    }
}

void scene_manager::render(const sdl::irenderer &renderer) const noexcept
{
    // Pass 1: render each scene into its own render target texture.
    for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
    {
        iscene *scene_ptr = it->get();
        auto &target = m_render_targets[scene_ptr];

        if (!target)
        {
            try
            {
                const auto &cam = (*it)->camera();
                target = renderer.create_texture(sdl::pixel_format::rgba8888, sdl::texture_access::target,
                                                 static_cast<int>(cam.viewport.x), static_cast<int>(cam.viewport.y));
                target->set_blend_mode(sdl::blend_mode::blend);

                // We are a pixel art game, I think this might be needed.
                target->set_scale_mode(sdl::scale_mode::nearest);
            }
            catch (...)
            {
                sdl::log_error("Failed to create render target for scene");
                continue;
            }
        }

        renderer.set_render_target(target.get());
        const auto bg = (*it)->background_color();
        renderer.set_color(bg.r, bg.g, bg.b, bg.a);
        renderer.clear();

        for (const auto &[id, entity_ptr] : (*it)->entities())
            if (entity_ptr)
                system::render_entity(renderer, *entity_ptr, (*it)->camera());
    }

    // Pass 2: composite all scene textures onto the window in the same order.
    renderer.set_render_target(nullptr);
    renderer.set_color(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
    renderer.clear();

    constexpr float vw = static_cast<float>(APPLICATION_LOGICAL_WIDTH);
    constexpr float vh = static_cast<float>(APPLICATION_LOGICAL_HEIGHT);

    std::unordered_set<iscene *> transition_targets;
    for (const auto &t : m_transitions)
        transition_targets.insert(t.to_scene);

    for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
    {
        auto map_it = m_render_targets.find(it->get());
        if (map_it == m_render_targets.end() || !map_it->second)
            continue;

        sdl::itexture &tex = *map_it->second;

        if (transition_targets.count(it->get()))
            continue; // rendered as part of from_scene's render_transition call

        auto trans_it = std::find_if(m_transitions.begin(), m_transitions.end(),
                                     [&](const auto &t) { return t.from_scene == it->get(); });
        if (trans_it != m_transitions.end())
        {
            auto to_tex_it = m_render_targets.find(trans_it->to_scene);
            if (to_tex_it != m_render_targets.end() && to_tex_it->second)
            {
                render_transition(renderer, tex, *to_tex_it->second, trans_it->transition.type, trans_it->progress(),
                                  vw, vh);
                continue;
            }
        }

        tex.set_alpha_mod(255);
        render_texture_options(renderer, tex).dstrect({0.0f, 0.0f, vw, vh}).render();
    }
}

void scene_manager::set_background_color(sdl::fcolor color) noexcept
{
    m_background_color = color;
}

void scene_manager::process_push_back(scene_request &req)
{
    if (!req.scene)
        return;
    req.scene->m_handle = req.assigned_handle;
    req.scene->on_attach(*this);
    req.scene->on_start(*this);
    m_stack.push_back(std::move(req.scene));
}

void scene_manager::process_push_front(scene_request &req)
{
    if (!req.scene)
        return;
    req.scene->m_handle = req.assigned_handle;
    req.scene->on_attach(*this);
    req.scene->on_start(*this);
    m_stack.push_front(std::move(req.scene));
}

void scene_manager::process_push_before(scene_request &req)
{
    if (!req.scene)
        return;
    auto it = std::find_if(m_stack.begin(), m_stack.end(), [&](const auto &s) { return s->type() == req.target_type; });
    req.scene->m_handle = req.assigned_handle;
    req.scene->on_attach(*this);
    req.scene->on_start(*this);
    if (it != m_stack.end())
        m_stack.insert(it, std::move(req.scene));
    else
        m_stack.push_front(std::move(req.scene));
}

void scene_manager::process_push_after(scene_request &req)
{
    if (!req.scene)
        return;
    auto it =
        std::find_if(m_stack.rbegin(), m_stack.rend(), [&](const auto &s) { return s->type() == req.target_type; });
    req.scene->m_handle = req.assigned_handle;
    req.scene->on_attach(*this);
    req.scene->on_start(*this);
    if (it != m_stack.rend())
        m_stack.insert(it.base(), std::move(req.scene));
    else
        m_stack.push_back(std::move(req.scene));
}

void scene_manager::process_pop_front()
{
    if (m_stack.empty())
        return;
    auto scene = std::move(m_stack.front());
    m_stack.pop_front();
    m_render_targets.erase(scene.get());
    scene->on_pause(*this);
    scene->on_detach(*this);
}

void scene_manager::process_pop_last()
{
    if (m_stack.empty())
        return;
    auto scene = std::move(m_stack.back());
    m_stack.pop_back();
    m_render_targets.erase(scene.get());
    scene->on_pause(*this);
    scene->on_detach(*this);
}

void scene_manager::process_pop_of_type(scene_request &req)
{
    auto it = std::find_if(m_stack.begin(), m_stack.end(), [&](const auto &s) { return s->type() == req.target_type; });
    if (it == m_stack.end())
    {
        return;
    }

    auto scene = std::move(*it);
    m_stack.erase(it);
    m_render_targets.erase(scene.get());
    scene->on_pause(*this);
    scene->on_detach(*this);
}

void scene_manager::process_emit_signal(scene_request &req)
{
    if (req.signal)
    {
        m_outgoing_signals.emplace_back(std::move(req.signal));
    }
}

void scene_manager::process_pop(scene_request &req)
{
    auto it =
        std::find_if(m_stack.begin(), m_stack.end(), [&](const auto &s) { return s->handle() == req.target_handle; });
    if (it == m_stack.end())
        return;
    auto scene = std::move(*it);
    m_stack.erase(it);
    m_render_targets.erase(scene.get());
    scene->on_pause(*this);
    scene->on_detach(*this);
}

void scene_manager::process_start_transition(scene_request &req)
{
    if (!req.scene)
        return;
    auto it =
        std::find_if(m_stack.begin(), m_stack.end(), [&](const auto &s) { return s->handle() == req.target_handle; });
    if (it == m_stack.end())
        return;
    iscene *from = it->get();
    bool already_transitioning = std::any_of(m_transitions.begin(), m_transitions.end(), [from](const auto &t)
                                             { return t.from_scene == from || t.to_scene == from; });
    if (already_transitioning)
    {
        sdl::log_error("Scene is one of the scenes already transitioning, ignoring...");
        return;
    }
    from->on_pause(*this);
    req.scene->m_handle = req.assigned_handle;
    req.scene->on_attach(*this);
    auto to_it = m_stack.insert(it, std::move(req.scene));
    m_transitions.push_back({from, to_it->get(), 0.0, req.transition});
}

void scene_manager::process_requests()
{
    std::queue<scene_request> requests_to_process;

    // Acquire the rest of the requests.
    // We only lock the mutex until we swap, so signals pushed during processing are not ignored.
    {
        std::lock_guard<std::mutex> lock(m_requests_mutex);
        if (m_requests.empty())
            return;
        requests_to_process.swap(m_requests);
    }

    while (!requests_to_process.empty())
    {
        scene_request req = std::move(requests_to_process.front());
        requests_to_process.pop();

        switch (req.type)
        {
        case scene_request_type::push_back:
            process_push_back(req);
            break;
        case scene_request_type::push_front:
            process_push_front(req);
            break;
        case scene_request_type::push_before:
            process_push_before(req);
            break;
        case scene_request_type::push_after:
            process_push_after(req);
            break;
        case scene_request_type::pop_front:
            process_pop_front();
            break;
        case scene_request_type::pop_last:
            process_pop_last();
            break;
        case scene_request_type::pop_of_type:
            process_pop_of_type(req);
            break;
        case scene_request_type::pop:
            process_pop(req);
            break;
        case scene_request_type::emit_signal:
            process_emit_signal(req);
            break;
        case scene_request_type::start_transition:
            process_start_transition(req);
            break;
        }
    }
}

void scene_manager::drain_signals(std::deque<std::unique_ptr<isignal>> &target)
{
    while (!m_outgoing_signals.empty())
    {
        target.emplace_back(std::move(m_outgoing_signals.front()));
        m_outgoing_signals.pop_front();
    }
}

bool scene_manager::unsubscribe(uint64_t id)
{
    bool removed = false;
    for (auto &[type, listeners] : m_listeners)
    {
        for (auto it = listeners.begin(); it != listeners.end();)
        {
            if (it->get_id() == id)
            {
                it = listeners.erase(it);
                removed = true;
            }
            else
            {
                ++it;
            }
        }
    }
    return removed;
}

void scene_manager::register_listener(signal_listener listener)
{
    m_listeners[listener.get_type_index()].insert(std::move(listener));
}

uint64_t scene_manager::next_listener_id() noexcept
{
    return ++m_listener_id_counter;
}

void scene_manager::propagate_signals(isignal &signal)
{
    // Indiscriminately pass down signals. Scenes have to be wary whether the signals are already cancelled or not.
    if (auto it = m_listeners.find(signal.type()); it != m_listeners.end())
    {
        for (auto &listener : it->second)
        {
            listener(signal);
        }
    }
}

} // namespace ccsakura
