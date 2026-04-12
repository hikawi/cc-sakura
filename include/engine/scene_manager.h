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
    emit_signal,
    start_transition,
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
    scene_transition transition{};
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
    bool start_transition(std::unique_ptr<iscene> to_scene, scene_type from_type, scene_transition transition) override;
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

    struct transition_state
    {
        iscene *from_scene = nullptr; ///< raw observer ptr — still owned by m_stack
        iscene *to_scene = nullptr;   ///< raw observer ptr — still owned by m_stack
        double elapsed = 0.0;
        scene_transition config{};

        bool active() const noexcept
        {
            return config.type != scene_transition_type::none && from_scene != nullptr;
        }
        double progress() const noexcept
        {
            return config.duration <= 0.0 ? 1.0 : std::clamp(elapsed / config.duration, 0.0, 1.0);
        }
    };

    sdl::fcolor m_background_color{1.0f, 1.0f, 1.0f, 1.0f};
    mutable std::unordered_map<iscene *, std::unique_ptr<sdl::itexture>> m_render_targets;
    transition_state m_transition{};
};

} // namespace ccsakura
