/**
 * \file engine/scene.h
 *
 * Provides a scene-based approach to rendering layers for the engine.
 */

#pragma once

#include "scenes/scene_id.h"
#include "sdl/sdl_render.h"
#include "signal.h"

#include <deque>
#include <memory>
#include <mutex>
#include <queue>

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
     */
    virtual scene_type type() const noexcept = 0;

    /**
     * \brief Called when the scene is added to the scene manager.
     */
    virtual void on_attach();

    /**
     * \brief Called when the scene is removed from the scene manager.
     */
    virtual void on_detach();

    /**
     * \brief Logic update phase.
     * \param dt Delta time in seconds.
     * \returns true if it allows ticks to go to scenes on lower layers.
     */
    virtual bool on_tick(const double dt) noexcept;

    /**
     * \brief Physical update phase.
     * \returns true if it allows ticks to go to scenes on lower layers.
     */
    virtual bool on_physical_tick() noexcept;

    /**
     * \brief Rendering phase.
     */
    virtual void on_render(const sdl::irenderer &renderer) const noexcept;
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
    void tick(const double dt) override;
    void physical_tick() override;
    void render(const sdl::irenderer &renderer) const noexcept override;
    void process_requests() override;

  private:
    std::mutex m_requests_mutex;
    std::queue<scene_request> m_requests;
    std::deque<std::unique_ptr<iscene>> m_stack;
};

} // namespace ccsakura
