#include "engine/scene.h"

#include "engine/collision.h"
#include "engine/component.h"
#include "engine/render.h"
#include "engine/sprite.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_surface.h"

#include <algorithm>
#include <cctype>

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

signal_binder::signal_binder(scene_context &ctx, std::function<void(uint64_t)> on_subscribe)
    : m_ctx(ctx), m_on_subscribe(std::move(on_subscribe))
{
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

void iscene::on_pause(scene_context &)
{
}

void iscene::on_start(scene_context &)
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

void iscene::on_collide(uint32_t a, uint32_t b, const collision &col) noexcept
{
    dispatch_collision_hooks(a, b, col);
}

void iscene::dispatch_collision_hooks(uint32_t a, uint32_t b, const collision &col) noexcept
{
    if (const auto it = m_collision_hooks.find({a, b}); it != m_collision_hooks.end())
        it->second(a, b, col);
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

signal_binder iscene::bind_signals(scene_context &ctx)
{
    return signal_binder(ctx, [this](uint64_t id) { m_signal_sub_ids.push_back(id); });
}

void iscene::unbind_signals(scene_context &ctx) noexcept
{
    for (const uint64_t id : m_signal_sub_ids)
    {
        ctx.unsubscribe(id);
    }
    m_signal_sub_ids.clear();
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

bool scene_manager::start_transition(std::unique_ptr<iscene> to_scene, scene_type from_type,
                                     scene_transition transition)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::start_transition, std::move(to_scene), from_type, nullptr, transition});
    return true;
}

bool scene_manager::emit_signal(std::unique_ptr<isignal> signal)
{
    std::lock_guard<std::mutex> lock(m_requests_mutex);
    m_requests.push({scene_request_type::emit_signal, nullptr, scene_type::dbg_none, std::move(signal)});
    return true;
}

void scene_manager::tick(const double dt)
{
    if (m_transition.active())
    {
        m_transition.elapsed += dt;
        if (m_transition.elapsed >= m_transition.config.duration)
        {
            iscene *from = m_transition.from_scene;
            iscene *to = m_transition.to_scene;
            m_transition = {}; // clear before on_detach to prevent re-entry

            auto it = std::find_if(m_stack.begin(), m_stack.end(), [from](const auto &s) { return s.get() == from; });
            if (it != m_stack.end())
            {
                auto scene = std::move(*it);
                m_stack.erase(it);
                m_render_targets.erase(scene.get());
                scene->on_detach(*this);
            }

            auto to_it = std::find_if(m_stack.begin(), m_stack.end(), [to](const auto &s) { return s.get() == to; });
            if (to_it != m_stack.end())
                to->on_start(*this);
        }
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
        std::vector<std::pair<uint32_t, const collider *>> collidables;
        for (const auto &[id, ent] : scene->entities())
        {
            if (const auto *hb = ent->get_component<components::hitbox>())
                collidables.push_back({id, &hb->get()});
        }

        // Narrow phase — broad phase can be inserted here in the future
        for (size_t i = 0; i < collidables.size(); ++i)
        {
            for (size_t j = i + 1; j < collidables.size(); ++j)
            {
                // Guarantee a < b ordering before calling on_collide
                const auto [id_i, coll_i] = collidables[i];
                const auto [id_j, coll_j] = collidables[j];

                const bool swapped = id_i > id_j;
                const uint32_t id_a = swapped ? id_j : id_i;
                const uint32_t id_b = swapped ? id_i : id_j;
                const collider *coll_a = swapped ? coll_j : coll_i;
                const collider *coll_b = swapped ? coll_i : coll_j;

                const auto col = coll_a->collides(*coll_b);
                if (col.is_colliding)
                    scene->on_collide(id_a, id_b, col);
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
                target = renderer.create_texture(sdl::pixel_format::rgba8888, sdl::texture_access::target,
                                                 static_cast<int>(m_camera.viewport.x),
                                                 static_cast<int>(m_camera.viewport.y));
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

        render_scene(renderer, **it);
    }

    // Pass 2: composite all scene textures onto the window in the same order.
    renderer.set_render_target(nullptr);
    renderer.set_color(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
    renderer.clear();

    const float vw = static_cast<float>(m_camera.viewport.x);
    const float vh = static_cast<float>(m_camera.viewport.y);

    for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
    {
        auto map_it = m_render_targets.find(it->get());
        if (map_it == m_render_targets.end() || !map_it->second)
            continue;

        sdl::itexture &tex = *map_it->second;

        if (m_transition.active())
        {
            if (it->get() == m_transition.to_scene)
                continue; // rendered as part of from's render_transition call

            if (it->get() == m_transition.from_scene)
            {
                auto to_it = m_render_targets.find(m_transition.to_scene);
                if (to_it != m_render_targets.end() && to_it->second)
                {
                    render_transition(renderer, tex, *to_it->second, m_transition.config.type, m_transition.progress(),
                                      vw, vh);
                    continue;
                }
            }
        }

        tex.set_alpha_mod(255);
        sdl::render_texture_options(renderer, tex).dstrect({0.0f, 0.0f, vw, vh}).render();
    }
}

void scene_manager::render_scene(const sdl::irenderer &renderer, const iscene &scene) const noexcept
{
    for (const auto &[id, entity_ptr] : scene.entities())
    {
        if (!entity_ptr)
        {
            continue;
        }

        const auto sprite = entity_ptr->get_component<components::sprite>();
        const auto hitbox = entity_ptr->get_component<components::hitbox>();
        const auto text = entity_ptr->get_component<components::text>();

        if (!sprite && !hitbox && !text)
        {
            continue;
        }

        const auto transform = entity_ptr->get_component<components::transform>();
        sdl::fpoint screen_pos{0.0f, 0.0f};
        if (transform)
        {
            if (transform->fixed)
                screen_pos = {static_cast<float>(transform->position.x), static_cast<float>(transform->position.y)};
            else
                screen_pos = world_to_screen(transform->position, m_camera);
        }
        else
        {
            if (sprite)
            {
                sdl::log_warn("Entity {} has no transform but has sprite component", id);
            }
            if (hitbox)
            {
                sdl::log_warn("Entity {} has no transform but has hitbox component", id);
            }
            if (text)
            {
                sdl::log_warn("Entity {} has no transform but has text component", id);
            }
        }

        if (sprite && sprite->spr)
        {
            const auto frame = sprite->spr->frame(sprite->frame_index);
            sprite->spr->render_options(renderer)
                .srcrect(sdl::frect(frame.frame))
                .dst(screen_pos)
                .rotate(transform ? transform->rotation : 0.0)
                .render_origin(sprite->origin)
                .render();
        }

        if (hitbox)
        {
            hitbox->get().render(renderer, m_camera);
        }

        if (text && !text->value.empty())
        {
            auto &font = sprite::named("font");

            constexpr float SPACE_ADVANCE = 4.0f;
            constexpr float PUNCT_GAP = 2.0f;
            constexpr float CHAR_GAP = 1.0f;

            // First pass: measure total block size for origin shifting.
            float width = 0.0f, height = 0.0f;
            const size_t len = text->value.size();
            for (size_t i = 0; i < len; i++)
            {
                const char c = text->value[i];
                if (c < 32 || c > 126)
                {
                    sdl::log_warn("Undisplayable char {} in entity {}", c, entity_ptr->id());
                    continue;
                }

                if (c == ' ')
                {
                    if (i + 1 < len)
                        width += SPACE_ADVANCE;
                    continue;
                }

                const auto &frame = font.frame(static_cast<uint32_t>(c - 32));
                height = std::max(height, static_cast<float>(frame.frame.h));
                width += static_cast<float>(frame.frame.w);
                if (i + 1 < len)
                    width += std::ispunct(static_cast<unsigned char>(c)) ? PUNCT_GAP : CHAR_GAP;
            }

            sdl::frect block{0.0f, 0.0f, width, height};
            shift_origin(block, text->origin);

            float render_x = screen_pos.x + block.x;
            float render_y = screen_pos.y + block.y;

            // Second pass: render each glyph.
            for (std::size_t i = 0; i < len; ++i)
            {
                const char c = text->value[i];
                if (c < 32 || c > 126)
                    continue;

                if (c == ' ')
                {
                    render_x += SPACE_ADVANCE;
                    continue;
                }

                const auto &f = font.frame(static_cast<uint32_t>(c - 32));

                font.render_options(renderer)
                    .srcrect(sdl::frect(f.frame))
                    .dst(sdl::fpoint{render_x + static_cast<float>(f.spr_source_size.x),
                                     render_y + static_cast<float>(f.spr_source_size.y)})
                    .color_mod(text->color)
                    .render();

                const float gap = std::ispunct(static_cast<unsigned char>(c)) ? PUNCT_GAP : CHAR_GAP;
                render_x += static_cast<float>(f.frame.w) + gap;
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
                req.scene->on_start(*this);
                m_stack.push_back(std::move(req.scene));
            }
            break;
        case scene_request_type::push_front:
            if (req.scene)
            {
                req.scene->on_attach(*this);
                req.scene->on_start(*this);
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
                req.scene->on_start(*this);
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
                req.scene->on_start(*this);
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
                m_render_targets.erase(scene.get());
                scene->on_pause(*this);
                scene->on_detach(*this);
            }
            break;
        case scene_request_type::pop_last:
            if (!m_stack.empty())
            {
                auto scene = std::move(m_stack.back());
                m_stack.pop_back();
                m_render_targets.erase(scene.get());
                scene->on_pause(*this);
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
                m_render_targets.erase(scene.get());
                scene->on_pause(*this);
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
        case scene_request_type::start_transition:
        {
            if (!req.scene || m_transition.active())
                break;

            auto it = std::find_if(m_stack.begin(), m_stack.end(),
                                   [&](const auto &s) { return s->type() == req.target_type; });
            if (it == m_stack.end())
                break;

            iscene *from = it->get();
            from->on_pause(*this);
            req.scene->on_attach(*this);
            auto to_it = m_stack.insert(it, std::move(req.scene));
            iscene *to = to_it->get();
            m_transition = {from, to, 0.0, req.transition};
            break;
        }
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
