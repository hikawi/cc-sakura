#include "engine/scene.h"

#include "engine/component.h"
#include "engine/render.h"
#include "engine/sprite.h"

#include <algorithm>

namespace ccsakura
{

scene_entity_builder::scene_entity_builder(uint32_t id, std::function<entity *(entity)> commit)
    : m_builder(id), m_commit(std::move(commit))
{
}

entity *scene_entity_builder::build()
{
    return m_commit(m_builder.build());
}

scene_entity_builder iscene::construct_entity(uint32_t id)
{
    return scene_entity_builder(id,
                                [this](entity e) -> entity *
                                {
                                    uint32_t eid = e.id();
                                    add_entity(std::move(e));
                                    return get_entity(eid);
                                });
}

intent_binder iscene::bind_intents(scene_context &ctx)
{
    return intent_binder(
        [this, &ctx](std::vector<std::pair<sdl::keycode, intent>> bindings)
        {
            m_intent_bindings = std::move(bindings);
            m_intent_sub_id = ctx.subscribe(listener_priority::normal, &iscene::on_intent_key, this);
        });
}

void iscene::on_attach(scene_context &)
{
}
void iscene::on_detach(scene_context &)
{
}

bool iscene::on_tick(scene_context &, const double) noexcept
{
    return true;
}

bool iscene::on_physical_tick(scene_context &) noexcept
{
    return true;
}

const std::unordered_map<uint32_t, std::unique_ptr<entity>> &iscene::entities() const noexcept
{
    return m_entities;
}

void iscene::unbind_intents(scene_context &ctx) noexcept
{
    ctx.unsubscribe(m_intent_sub_id);
    m_intent_bindings.clear();
    m_intent_state = {};
}

bool iscene::is_intent_triggered(const intent i) const noexcept
{
    return m_intent_state[static_cast<std::size_t>(i)];
}

void iscene::on_intent_key(signals::key &e) noexcept
{
    if (e.is_cancelled())
        return;
    for (auto &[key, i] : m_intent_bindings)
    {
        if (e.keycode == key)
        {
            m_intent_state[static_cast<std::size_t>(i)] = e.down;
            return;
        }
    }
}

void iscene::add_entity(entity &&e)
{
    uint32_t id = e.id();
    m_entities[id] = std::make_unique<entity>(std::move(e));
}

entity *iscene::get_entity(uint32_t id) noexcept
{
    const auto it = m_entities.find(id);
    return it != m_entities.end() ? it->second.get() : nullptr;
}

const entity *iscene::get_entity(uint32_t id) const noexcept
{
    const auto it = m_entities.find(id);
    return it != m_entities.end() ? it->second.get() : nullptr;
}

bool iscene::remove_entity(uint32_t id) noexcept
{
    return m_entities.erase(id) > 0;
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

void scene_manager::render(const sdl::irenderer &renderer) const noexcept
{
    renderer.set_color(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
    renderer.clear();

    for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
    {
        for (const auto &[id, entity_ptr] : (*it)->entities())
        {
            if (!entity_ptr)
            {
                continue;
            }
            const auto transform = entity_ptr->get_component<components::transform>();
            if (!transform)
            {
                continue;
            }

            const auto sprite = entity_ptr->get_component<components::sprite>();
            if (sprite && sprite->spr)
            {
                const auto frame = sprite->spr->frame(sprite->frame_index);
                sprite->spr->render_options(renderer)
                    .srcrect(sdl::frect(frame.frame))
                    .dst(world_to_screen(transform->position, m_camera))
                    .rotate(transform->rotation)
                    .render_origin(sprite->origin)
                    .render();
            }

            // TODO: Add a mechanism to disable debug-mode rendering for colliders.
            const auto hitbox = entity_ptr->get_component<components::hitbox>();
            if (hitbox)
            {
                hitbox->get().render(renderer, m_camera);
            }
        }
    }
}

camera2d &scene_manager::camera() noexcept
{
    return m_camera;
}

void scene_manager::set_background_color(sdl::fcolor color) noexcept
{
    m_background_color = color;
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
                req.scene->on_attach(*this);
                m_stack.push_back(std::move(req.scene));
            }
            break;
        case scene_request_type::push_front:
            if (req.scene)
            {
                req.scene->on_attach(*this);
                m_stack.push_front(std::move(req.scene));
            }
            break;
        case scene_request_type::push_before:
        {
            if (req.scene)
            {
                auto it = std::find_if(m_stack.begin(), m_stack.end(),
                                       [&](const auto &s) { return s->type() == req.target_type; });
                req.scene->on_attach(*this);
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
                req.scene->on_attach(*this);
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
                scene->on_detach(*this);
            }
            break;
        case scene_request_type::pop_last:
            if (!m_stack.empty())
            {
                auto scene = std::move(m_stack.back());
                m_stack.pop_back();
                scene->on_detach(*this);
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
                scene->on_detach(*this);
            }
            break;
        }
        case scene_request_type::emit_signal:
            if (req.signal)
            {
                m_outgoing_signals.emplace_back(std::move(req.signal));
            }
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
