/**
 * \file engine/scene.h
 *
 * Provides a scene-based approach to rendering layers for the engine.
 */

#pragma once

#include "entity.h"
#include "intent.h"
#include "scenes/scene_id.h"
#include "sdl/sdl_render.h"
#include "signal.h"

#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ccsakura
{

class iscene_manager;
class iscene;
class scene_context;

/**
 * Represents a proxy of scene manager to allow scenes to manipulate the scenes stack
 * queue their requests, instead of breaking the normal operation flow.
 */
class scene_context
{
  public:
    virtual ~scene_context() = default;

    /**
     * \brief Queues a request to add a new scene.
     * \param scene The scene to add.
     * \returns true if it succeeded, false otherwise.
     */
    virtual bool push_back(std::unique_ptr<iscene> scene) = 0;

    /**
     * \brief Queues a request to push a scene to the front of the stack.
     * \param scene The scene to add.
     * \returns true if it succeeded, false otherwise.
     */
    virtual bool push_front(std::unique_ptr<iscene> scene) = 0;

    /**
     * \brief Queues a request to push a scene before the first scene found of that type.
     *
     * If there is no scene with that type, acts the same as `push_front`.
     *
     * \param scene the scene to add.
     * \param type the type of scene to push before.
     * \returns true if it succeeded, false otherwise.
     */
    virtual bool push_before(std::unique_ptr<iscene> scene, const scene_type type) = 0;

    /**
     * \brief Queues a request to push a scene right after the last scene found of that type.
     *
     * If there is no scene with that type, acts the same as `push_back`.
     *
     * \param scene the scene to add.
     * \param type the type of scene to push before.
     * \returns true if it succeeded, false otherwise.
     */
    virtual bool push_after(std::unique_ptr<iscene> scene, const scene_type type) = 0;

    /**
     * \brief Pops the first scene of the stack.
     *
     * If the stack is empty, returns nullptr.
     *
     * \returns the scene ownership, null if it is empty.
     */
    virtual std::unique_ptr<iscene> pop_front() = 0;

    /**
     * \brief Pops the last scene of the stack.
     *
     * If the stack is empty, returns nullptr.
     *
     * \returns the scene ownership, null if it is empty.
     */
    virtual std::unique_ptr<iscene> pop_last() = 0;

    /**
     * \brief Pops the first scene of the stack with type.
     *
     * If the stack is empty, returns nullptr.
     *
     * \param type the type of scene to pop
     * \returns the scene ownership, null if it is not found.
     */
    virtual std::unique_ptr<iscene> pop_of_type(const scene_type type) = 0;

    /**
     * Emits a signal.
     *
     * \param signal the signal to emit
     * \returns true if the request was queued, false otherwise.
     */
    virtual bool emit_signal(std::unique_ptr<isignal> signal) = 0;

    /**
     * Unsubscribes a previously registered listener by its ID.
     *
     * \param id the subscription ID
     * \returns true if removed successfully, false if not found.
     */
    virtual bool unsubscribe(uint64_t id) = 0;

    /**
     * Returns the active camera for this scene stack.
     *
     * Be careful of modifying this, because this might affect the render pass for ALL scenes,
     * not just one.
     *
     * \returns the current camera instance
     */
    virtual camera2d &camera() noexcept = 0;

    /**
     * Sets the background color used to clear the screen each frame.
     *
     * \param color the desired background color
     */
    virtual void set_background_color(sdl::fcolor color) noexcept = 0;

    /**
     * Subscribes to a signal type at priority.
     *
     * \tparam T the signal type to subscribe to
     * \param priority the listener priority
     * \param callback the function to call when the signal is emitted
     * \returns the new subscriber id
     */
    template <typename T>
        requires std::derived_from<T, isignal>
    uint64_t subscribe(listener_priority priority, std::function<void(T &)> callback)
    {
        uint64_t id = next_listener_id();
        signal_listener listener(priority, id, callback);
        register_listener(std::move(listener));
        return id;
    }

    /**
     * Subscribes to a signal type at priority using a member function.
     *
     * \tparam T the signal type to subscribe to
     * \tparam Class the class type containing the method
     * \param priority the listener priority
     * \param method the member function to call
     * \param instance the class instance to call the method on
     * \returns the new subscriber id
     */
    template <typename T, typename Class>
        requires std::derived_from<T, isignal>
    uint64_t subscribe(listener_priority priority, void (Class::*method)(T &), Class *instance)
    {
        return subscribe<T>(priority, [instance, method](T &ev) { (instance->*method)(ev); });
    }

  protected:
    virtual uint64_t next_listener_id() noexcept = 0;
    virtual void register_listener(signal_listener listener) = 0;
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
     * Returns a const reference to this scene's entity registry.
     *
     * \returns a const reference to the entities map
     */
    const std::unordered_map<uint32_t, std::unique_ptr<entity>> &entities() const noexcept;

  protected:
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

  private:
    void on_intent_key(signals::key &e) noexcept;

    std::vector<std::pair<sdl::keycode, intent>> m_intent_bindings;
    intent_state m_intent_state{};
    uint64_t m_intent_sub_id{0};

  protected:
    std::unordered_map<uint32_t, std::unique_ptr<entity>> m_entities; ///< Entity registry
};

/**
 * \brief Manages the lifecycle and rendering of game scenes.
 *
 * This interface defines how the engine interacts with the scene management system.
 * It supports adding and removing scenes, processing updates, handling events, and rendering.
 */
class iscene_manager : public scene_context
{
  public:
    virtual ~iscene_manager() = default;

    /**
     * \brief Updates the logic of all active scenes.
     *
     * Processes any pending scene change requests after the update loop.
     *
     * \param dt Delta time in seconds.
     */
    virtual void tick(const double dt) = 0;

    /**
     * \brief Updates the physics of all active scenes.
     */
    virtual void physical_tick() = 0;

    /**
     * \brief Renders all active scenes in order.
     * \param renderer The renderer to use.
     */
    virtual void render(const sdl::irenderer &renderer) const noexcept = 0;

    virtual camera2d &camera() noexcept = 0;
    virtual void set_background_color(sdl::fcolor color) noexcept = 0;

    /**
     * Processes all pending requests.
     */
    virtual void process_requests() = 0;

    /**
     * Drains all emitted signals into the target deque.
     *
     * \param target the deque to move signals into
     */
    virtual void drain_signals(std::deque<std::unique_ptr<isignal>> &target) = 0;

    /**
     * Propagates a signal down to all subscribers.
     *
     * \param signal the signal to propagate
     */
    virtual void propagate_signals(isignal &signal) = 0;
};

enum class scene_request_type
{
    push_back,
    push_front,
    push_before,
    push_after,
    pop_front,
    pop_last,
    pop_of_type,
    emit_signal,
};

/**
 * Internal struct for scene managers.
 */
struct scene_request
{
    scene_request_type type;
    std::unique_ptr<iscene> scene = nullptr;
    scene_type target_type = scene_type::dbg_none;
    std::unique_ptr<isignal> signal = nullptr;
};

/**
 * \brief Provides a concrete manager of scenes.
 */
class scene_manager : public iscene_manager
{
  public:
    bool push_back(std::unique_ptr<iscene> scene) override;
    bool push_front(std::unique_ptr<iscene> scene) override;
    bool push_before(std::unique_ptr<iscene> scene, const scene_type type) override;
    bool push_after(std::unique_ptr<iscene> scene, const scene_type type) override;
    std::unique_ptr<iscene> pop_front() override;
    std::unique_ptr<iscene> pop_last() override;
    std::unique_ptr<iscene> pop_of_type(const scene_type type) override;
    bool emit_signal(std::unique_ptr<isignal> signal) override;
    bool unsubscribe(uint64_t id) override;

    void tick(const double dt) override;
    void physical_tick() override;
    void render(const sdl::irenderer &renderer) const noexcept override;
    void process_requests() override;
    void drain_signals(std::deque<std::unique_ptr<isignal>> &target) override;
    void propagate_signals(isignal &signal) override;
    camera2d &camera() noexcept override;
    void set_background_color(sdl::fcolor color) noexcept override;

  protected:
    uint64_t next_listener_id() noexcept override;
    void register_listener(signal_listener listener) override;

  private:
    std::mutex m_requests_mutex;
    std::queue<scene_request> m_requests;
    std::deque<std::unique_ptr<iscene>> m_stack;
    std::deque<std::unique_ptr<isignal>> m_outgoing_signals;
    std::unordered_map<std::type_index, std::set<signal_listener>> m_listeners;

    std::atomic<uint64_t> m_listener_id_counter{0};

    camera2d m_camera{{240.0, 135.0}, 0.0, 1.0, {480.0, 270.0}};
    sdl::fcolor m_background_color{1.0f, 1.0f, 1.0f, 1.0f};
};

} // namespace ccsakura
