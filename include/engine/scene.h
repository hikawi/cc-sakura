/**
 * \file engine/scene.h
 *
 * Provides a scene-based approach to rendering layers for the engine.
 */

#pragma once

#include "intent.h"
#include "scenes/scene_id.h"
#include "sdl/sdl_render.h"
#include "signal.h"

#include <atomic>
#include <deque>
#include <initializer_list>
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
     * \brief Rendering phase.
     * \param renderer The renderer to use.
     */
    virtual void on_render(const sdl::irenderer &renderer) const noexcept;

  protected:
    /**
     * \brief Registers intent bindings and subscribes to key signals.
     *
     * Call in on_attach. The scene is the context: bindings are active only while subscribed.
     *
     * \param ctx the scene context
     * \param bindings key-to-intent pairs
     */
    void bind_intents(scene_context &ctx,
                      std::initializer_list<std::pair<sdl::keycode, intent>> bindings) noexcept;

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
    bool is_intent_triggered(intent i) const noexcept;

  private:
    void on_intent_key(signals::key &e) noexcept;

    std::vector<std::pair<sdl::keycode, intent>> m_intent_bindings;
    intent_state m_intent_state{};
    uint64_t m_intent_sub_id{0};
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
};

} // namespace ccsakura
