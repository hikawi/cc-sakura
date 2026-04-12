/**
 * \file engine/scene.h
 *
 * Provides a scene-based approach to rendering layers for the engine.
 */

#pragma once

#include "engine/camera.h"
#include "engine/collision.h"
#include "engine/entity.h"
#include "engine/intent.h"
#include "engine/scene_context.h"

#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ccsakura
{

/**
 * Fluent builder that registers signal listeners and tracks their IDs.
 * Obtain via iscene::bind_signals() rather than constructing directly.
 */
class signal_binder
{
  public:
    signal_binder(scene_context &ctx, std::function<void(uint64_t)> on_subscribe);

    /**
     * Subscribes to a signal type at priority.
     *
     * \tparam T the signal type to subscribe to
     * \param priority the listener priority
     * \param callback the function to call when the signal is emitted
     * \returns this builder for chaining
     */
    template <typename T>
        requires std::derived_from<T, isignal>
    signal_binder &on(listener_priority priority, std::function<void(T &)> callback)
    {
        uint64_t id = m_ctx.subscribe<T>(priority, std::move(callback));
        m_on_subscribe(id);
        return *this;
    }

    /**
     * Subscribes to a signal type using a member function.
     *
     * \tparam T the signal type to subscribe to
     * \tparam Class the class type containing the method
     * \param priority the listener priority
     * \param method the member function to call
     * \param instance the class instance to call the method on
     * \returns this builder for chaining
     */
    template <typename T, typename Class>
        requires std::derived_from<T, isignal>
    signal_binder &on(listener_priority priority, void (Class::*method)(T &), Class *instance)
    {
        return on<T>(priority, [instance, method](T &ev) { (instance->*method)(ev); });
    }

    /**
     * Commits the builder. No-op since subscriptions are registered immediately,
     * but provided for consistency with intent_binder.
     */
    void bind()
    {
    }

  private:
    scene_context &m_ctx;
    std::function<void(uint64_t)> m_on_subscribe;
};

/**
 * Fluent builder that registers the entity directly into a scene on build().
 * Obtain via iscene::construct_entity() rather than constructing directly.
 */
class scene_entity_builder
{
  public:
    scene_entity_builder(uint32_t id, std::function<entity *(entity)> commit);

    /**
     * Adds a component to the entity under construction.
     *
     * \tparam T the component type
     * \param args arguments forwarded to the component constructor
     * \returns this builder for chaining
     */
    template <typename T, typename... Args>
        requires(std::is_base_of_v<component, T>)
    scene_entity_builder &with_component(Args &&...args)
    {
        m_builder.with_component<T>(std::forward<Args>(args)...);
        return *this;
    }

    /**
     * Applies a function to an already-added component, allowing mutation before build.
     *
     * \tparam T the component type — must have been added via with_component first
     * \param fn callable receiving a \c T& reference
     * \returns this builder for chaining
     */
    template <typename T, typename Fn>
        requires(std::is_base_of_v<component, T>)
    scene_entity_builder &edit_component(Fn &&fn)
    {
        m_builder.edit_component<T>(std::forward<Fn>(fn));
        return *this;
    }

    /**
     * Finalises the entity, registers it in the scene, and returns a raw pointer to it.
     *
     * \returns pointer to the entity now owned by the scene, or nullptr if not found
     */
    entity *build();

  private:
    entity_builder m_builder;
    std::function<entity *(entity)> m_commit;
};

/**
 * \brief Interface for a game scene.
 *
 * Scenes represent distinct states or layers of the game (e.g., menus, levels, HUD).
 */
class iscene
{
  public:
    virtual ~iscene() = default;

    /**
     * \brief Returns the type of the scene.
     * \returns the scene type
     */
    virtual scene_type type() const noexcept = 0;

    /**
     * \brief Called when the scene is added to the scene manager.
     * \param ctx the scene context for queuing further requests
     */
    virtual void on_attach(scene_context &ctx);

    /**
     * \brief Called when the scene is removed from the scene manager.
     * \param ctx the scene context for queuing further requests
     */
    virtual void on_detach(scene_context &ctx);

    /**
     * \brief Called when a transition targeting this scene begins.
     *
     * If this scene was popped by a request without a scene transition,
     * this will get called instantly before `on_detach`.
     *
     * \param ctx the scene context for queuing further requests
     */
    virtual void on_pause(scene_context &ctx);

    /**
     * \brief Called when this scene has fully transitioned in and is now active.
     *
     * When used with push_front or push_back, this gets called instantly.
     *
     * \param ctx the scene context for queuing further requests
     */
    virtual void on_start(scene_context &ctx);

    /**
     * \brief Logic update phase.
     * \param ctx the scene context for queuing further requests
     * \param dt Delta time in seconds.
     * \returns true if it allows ticks to go to scenes on lower layers.
     */
    virtual bool on_tick(scene_context &ctx, const double dt) noexcept;

    /**
     * \brief Physical update phase.
     * \param ctx the scene context for queuing further requests
     * \returns true if it allows ticks to go to scenes on lower layers.
     */
    virtual bool on_physical_tick(scene_context &ctx) noexcept;

    /**
     * \brief Called once per colliding entity pair after the physics step.
     *
     * Pair ordering is guaranteed: \c a < \c b (by entity id).
     * \c col.normal is from \c a 's perspective (result of a.collides(b)).
     *
     * Default implementation dispatches to hooks registered via register_collision_pair().
     * Override for full custom control; call dispatch_collision_hooks() to retain hook behaviour.
     *
     * \param a the entity id with the lower value
     * \param b the entity id with the higher value
     * \param col the collision result
     */
    virtual void on_collide(uint32_t a, uint32_t b, const collision &col) noexcept;

    /**
     * Returns a const reference to this scene's entity registry.
     *
     * \returns a const reference to the entities map
     */
    const std::unordered_map<uint32_t, std::unique_ptr<entity>> &entities() const noexcept;

    /**
     * Returns the background color used to clear this scene's render target each frame.
     *
     * \returns the scene's background color
     */
    sdl::fcolor background_color() const noexcept
    {
        return m_background_color;
    }

    /**
     * Returns this scene's camera.
     *
     * \returns a reference to the per-scene camera
     */
    camera2d &camera() noexcept
    {
        return m_camera;
    }

    /**
     * Returns this scene's camera.
     *
     * \returns a const reference to the per-scene camera
     */
    const camera2d &camera() const noexcept
    {
        return m_camera;
    }

  protected:
    sdl::fcolor m_background_color{0.0f, 0.0f, 0.0f, 0.0f}; ///< Per-scene clear color
    camera2d m_camera{{APPLICATION_LOGICAL_WIDTH / 2.0, APPLICATION_LOGICAL_HEIGHT / 2.0},
                      0.0,
                      1.0,
                      {APPLICATION_LOGICAL_WIDTH, APPLICATION_LOGICAL_HEIGHT}}; ///< Per-scene camera
    /**
     * Adds an entity to the registry, keyed by its ID.
     * Replaces any existing entity with the same ID.
     *
     * \param e the entity to add
     */
    void add_entity(entity &&e);

    /**
     * Creates a fluent entity builder that registers the entity into this scene on build().
     *
     * \param id the entity ID
     * \returns a scene_entity_builder chained to this scene
     */
    scene_entity_builder construct_entity(uint32_t id);

    /**
     * Returns a pointer to the entity with the given ID, or nullptr if not found.
     *
     * \param id the entity ID
     * \returns an entity pointer
     */
    entity *get_entity(uint32_t id) noexcept;

    /**
     * Returns a pointer to the entity with the given ID, or nullptr if not found.
     *
     * \param id the entity ID
     * \returns an entity pointer
     */
    const entity *get_entity(uint32_t id) const noexcept;

    /**
     * Removes the entity with the given ID from the registry.
     *
     * \param id the entity ID
     * \returns true if an entity was removed, false if not found
     */
    bool remove_entity(uint32_t id) noexcept;

    /**
     * Returns a reference to a component on the entity with the given ID,
     * or nullptr if the entity or component is not found.
     *
     * \tparam T the component type
     * \param id the entity ID
     * \returns a pointer to the component
     */
    template <typename T>
        requires(std::is_base_of_v<component, T>)
    T *get_entity_component(uint32_t id) noexcept
    {
        auto *e = get_entity(id);
        if (!e || !e->has_component<T>())
        {
            return nullptr;
        }
        return e->get_component<T>();
    }

    /**
     * Returns a reference to a component on the entity with the given ID,
     * or nullptr if the entity or component is not found.
     *
     * \tparam T the component type
     * \param id the entity ID
     * \returns the const pointer to the component if found
     */
    template <typename T>
        requires(std::is_base_of_v<component, T>)
    const T *get_entity_component(uint32_t id) const noexcept
    {
        auto *e = get_entity(id);
        if (!e || !e->has_component<T>())
        {
            return nullptr;
        }
        return e->get_component<T>();
    }

    /**
     * Returns a tuple of pointers to the requested components for the given ID.
     * If a component or the entity is not found, its pointer will be nullptr.
     *
     * \tparam Comps the component types to retrieve
     * \param id the entity ID
     * \returns a tuple of pointers
     */
    template <typename... Comps>
        requires((std::is_base_of_v<component, Comps>) && ...)
    std::tuple<Comps *...> get_entity_components(uint32_t id) noexcept
    {
        auto *e = get_entity(id);
        if (!e)
        {
            return {static_cast<Comps *>(nullptr)...};
        }
        return {e->get_component<Comps>()...};
    }

    /**
     * Returns a tuple of pointers to the requested components for the given ID.
     * If a component or the entity is not found, its pointer will be nullptr.
     *
     * \tparam Comps the component types to retrieve
     * \param id the entity ID
     * \returns a tuple of pointers
     */
    template <typename... Comps>
        requires((std::is_base_of_v<component, Comps>) && ...)
    std::tuple<const Comps *...> get_entity_components(uint32_t id) const noexcept
    {
        const auto *e = get_entity(id);
        if (!e)
        {
            return {static_cast<const Comps *>(nullptr)...};
        }
        return {e->get_component<Comps>()...};
    }

    /**
     * Safely executes a function with the requested components if they are all present.
     *
     * \tparam Comps the component types to retrieve
     * \tparam Fn callable taking references to the components: void(Comps&...)
     * \param id the entity ID
     * \param fn the function to execute
     */
    template <typename... Comps, typename Fn>
        requires((std::is_base_of_v<component, Comps>) && ...)
    void with_entity_components(uint32_t id, Fn &&fn)
    {
        auto components = get_entity_components<Comps...>(id);
        bool all_exist = std::apply([](auto *...pts) { return (pts && ...); }, components);
        if (all_exist)
        {
            std::apply([&fn](auto *...pts) { fn(*pts...); }, components);
        }
    }

    /**
     * Safely executes a function with the requested components if they are all present.
     *
     * \tparam Comps the component types to retrieve
     * \tparam Fn callable taking const references to the components: void(const Comps&...)
     * \param id the entity ID
     * \param fn the function to execute
     */
    template <typename... Comps, typename Fn>
        requires((std::is_base_of_v<component, Comps>) && ...)
    void with_entity_components(uint32_t id, Fn &&fn) const
    {
        auto components = get_entity_components<Comps...>(id);
        bool all_exist = std::apply([](auto *...pts) { return (pts && ...); }, components);
        if (all_exist)
        {
            std::apply([&fn](auto *...pts) { fn(*pts...); }, components);
        }
    }

    using collision_hook_fn = std::function<void(uint32_t, uint32_t, const collision &)>;

    /**
     * Registers a callback invoked by the default on_collide() when entities with
     * ids \c a and \c b collide. Pair order does not matter.
     *
     * \tparam Fn any callable with signature void(uint32_t, uint32_t, const collision &)
     * \param a first entity id
     * \param b second entity id
     * \param fn the callback to invoke
     */
    template <typename Fn> void register_collision_pair(uint32_t a, uint32_t b, Fn &&fn)
    {
        this->m_collision_hooks[{std::min(a, b), std::max(a, b)}] = std::forward<Fn>(fn);
    }

    /**
     * Dispatches the collision to the registered hook for this pair, if any.
     *
     * Call this inside an on_collide() override to retain hook behaviour
     * while adding catch-all logic before or after it.
     *
     * \param a the entity id with the lower value
     * \param b the entity id with the higher value
     * \param col the collision result
     */
    void dispatch_collision_hooks(uint32_t a, uint32_t b, const collision &col) noexcept;

    /**
     * \brief Returns a fluent builder for registering key-to-intent bindings.
     *
     * Call \c .map().bind() in on_attach. Bindings are active only while the subscription stands.
     *
     * \param ctx the scene context
     * \returns an intent_binder for chaining
     */
    intent_binder bind_intents(scene_context &ctx);

    /**
     * \brief Unregisters intent bindings and unsubscribes from key signals.
     *
     * Call in on_detach.
     *
     * \param ctx the scene context
     */
    void unbind_intents(scene_context &ctx) noexcept;

    /**
     * \brief Returns whether the given intent is currently active.
     * \param i the intent to query
     * \returns true if the intent is triggered
     */
    bool is_intent_triggered(const intent i) const noexcept;

    /**
     * \brief Returns a fluent builder for registering signal listeners.
     *
     * Call \c .on().bind() in on_attach. Subscriptions are tracked and removed in \c unbind_signals.
     *
     * \param ctx the scene context
     * \returns a signal_binder for chaining
     */
    signal_binder bind_signals(scene_context &ctx);

    /**
     * \brief Unregisters all signal listeners registered via bind_signals.
     *
     * Call in on_detach.
     *
     * \param ctx the scene context
     */
    void unbind_signals(scene_context &ctx) noexcept;

  private:
    void on_intent_key(signals::key &e) noexcept;

    std::vector<std::pair<sdl::keycode, intent>> m_intent_bindings;
    intent_state m_intent_state{};
    uint64_t m_intent_sub_id{0};
    std::vector<uint64_t> m_signal_sub_ids;
    std::map<std::pair<uint32_t, uint32_t>, collision_hook_fn> m_collision_hooks;

  protected:
    std::unordered_map<uint32_t, std::unique_ptr<entity>> m_entities; ///< Entity registry
};

/**
 * Registers a collision hook inside a scene constructor or on_attach.
 *
 * Available identifiers inside \c body:
 *   - \c uint32_t a  — the entity with the smaller id
 *   - \c uint32_t b  — the entity with the larger id
 *   - \c const collision& col — the collision result (normal is from a's perspective)
 */
#define ON_COLLISION(id_a, id_b, body)                                                                                 \
    register_collision_pair((id_a), (id_b), [this](uint32_t a, uint32_t b, const collision &col) noexcept body)

} // namespace ccsakura
