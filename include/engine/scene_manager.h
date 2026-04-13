/**
 * \file engine/scene_manager.h
 *
 * Provides the scene manager interface and concrete implementation.
 */

#pragma once

#include "engine/scene.h"
#include "sdl/sdl_render.h"

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ccsakura
{

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
     * \brief Runs narrow-phase collision detection across all scenes,
     *        calling on_collide() for each colliding entity pair.
     *
     * Broad-phase optimisations (culling, quadtrees) can be added inside
     * the implementation without touching individual scenes.
     */
    virtual void collision_tick() = 0;

    /**
     * \brief Renders all active scenes in order.
     * \param renderer The renderer to use.
     */
    virtual void render(const sdl::irenderer &renderer) const noexcept = 0;

    /**
     * Sets the background color for a clear.
     *
     * \param color the color to set to.
     */
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
    pop,
    emit_signal,
    start_transition,
};

/**
 * Internal struct for scene managers.
 */
struct scene_request
{
    scene_request_type type;
    std::unique_ptr<iscene> scene   = nullptr;
    scene_type target_type          = scene_type::none;
    std::unique_ptr<isignal> signal = nullptr;
    scene_transition transition{};
    scene_handle assigned_handle    = invalid_scene_handle; ///< handle given to the new scene being pushed
    scene_handle target_handle      = invalid_scene_handle; ///< handle of the existing scene to operate on
};

struct transition_state
{
    iscene *from_scene = nullptr; ///< raw observer ptr — still owned by m_stack
    iscene *to_scene = nullptr;   ///< raw observer ptr — still owned by m_stack
    double elapsed = 0.0;
    scene_transition transition{};

    double progress() const noexcept
    {
        return transition.duration <= 0.0 ? 1.0 : std::clamp(elapsed / transition.duration, 0.0, 1.0);
    }
};

/**
 * \brief Provides a concrete manager of scenes.
 */
class scene_manager : public iscene_manager
{
  public:
    scene_handle push_back(std::unique_ptr<iscene> scene) override;
    scene_handle push_front(std::unique_ptr<iscene> scene) override;
    scene_handle push_before(std::unique_ptr<iscene> scene, const scene_type type) override;
    scene_handle push_after(std::unique_ptr<iscene> scene, const scene_type type) override;
    void pop_front() override;
    void pop_last() override;
    void pop_of_type(const scene_type type) override;
    scene_handle start_transition(std::unique_ptr<iscene> to_scene, scene_handle from,
                                  scene_transition transition) override;
    void pop(scene_handle handle) override;
    bool emit_signal(std::unique_ptr<isignal> signal) override;
    bool unsubscribe(uint64_t id) override;

    void tick(const double dt) override;
    void physical_tick() override;
    void collision_tick() override;
    void render(const sdl::irenderer &renderer) const noexcept override;
    void process_requests() override;
    void drain_signals(std::deque<std::unique_ptr<isignal>> &target) override;
    void propagate_signals(isignal &signal) override;
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
    std::atomic<scene_handle> m_scene_handle_counter{0};

    sdl::fcolor m_background_color{1.0f, 1.0f, 1.0f, 1.0f};
    mutable std::unordered_map<iscene *, std::unique_ptr<sdl::itexture>> m_render_targets;
    std::vector<transition_state> m_transitions;

    void process_push_back(scene_request &req);
    void process_push_front(scene_request &req);
    void process_push_before(scene_request &req);
    void process_push_after(scene_request &req);
    void process_pop_front();
    void process_pop_last();
    void process_pop_of_type(scene_request &req);
    void process_pop(scene_request &req);
    void process_emit_signal(scene_request &req);
    void process_start_transition(scene_request &req);
};

} // namespace ccsakura
