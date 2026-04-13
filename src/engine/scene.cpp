#include "engine/scene.h"

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

} // namespace ccsakura
