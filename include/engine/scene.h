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

namespace ccsakura
{

/**
 * \brief Interface for a game scene.
 *
 * Scenes represent distinct states or layers of the game (e.g., menus, levels, HUD).
 * Use the constructor for one-time initialization and destructor for cleanup.
 */
class iscene
{
  public:
    virtual ~iscene() = default;

    /**
     * \brief Returns the type of the scene.
     *
     * \returns the scene type
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
     * \brief Logic update phase. Called every frame with variable time step.
     * \param dt Delta time in seconds.
     * \returns true if it allows ticks to go downwards
     */
    virtual bool on_tick(const double dt) noexcept;

    /**
     * \brief Physical update phase. Called once every fixed time step (approx. 16.6ms).
     * \returns true if it allows physical ticks to go downwards
     */
    virtual bool on_physical_tick() noexcept;

    /**
     * \brief Signal/Event processing phase.
     * \param signal The signal to process.
     * \return true if it allows signals to propagate downwards
     */
    virtual bool on_signal(isignal &signal) noexcept;

    /**
     * \brief Rendering phase.
     */
    virtual void on_render(const sdl::irenderer &renderer) const noexcept;
};

/**
 * Provides a concrete manager of scenes, and is the one responsible for calling up scene's hooks.
 */
class scene_manager
{
  public:
    /**
     * Enqueues a scene to the front of the scene queue.
     *
     * \param scene the ownership of a scene
     * \returns a null pointer if it was successfully transferred, the same scene otherwise
     */
    std::unique_ptr<iscene> push_front(std::unique_ptr<iscene> scene) noexcept;

    /**
     * Enqueues a scene to the back of the scene queue.
     *
     * \param scene the ownership of a scene
     * \returns a null pointer if it was successfully transferred, the same scene otherwise
     */
    std::unique_ptr<iscene> push_back(std::unique_ptr<iscene> scene) noexcept;

    /**
     * Pops the front scene out of the queue. This calls on_detach().
     *
     * \returns a null pointer if the queue was empty, the scene otherwise.
     */
    std::unique_ptr<iscene> pop_front() noexcept;

    /**
     * Pops the back scene out of the queue. This calls on_detach().
     *
     * \returns a null pointer if the queue was empty, the scene otherwise.
     */
    std::unique_ptr<iscene> pop_back() noexcept;

    /**
     * Ticks all scenes in order of top to bottom. If a scene is modal, scenes below will
     * not get ticked.
     */
    void tick(const double dt) const noexcept;

    /**
     * Ticks all scenes in order of top to bottom for a physics-updating tick. If a scene is
     * modal, scenes below will not get ticked.
     */
    void physical_tick() const noexcept;

    /**
     * Renders all scenes from bottom to top. A scene is ignored if `scene.is_enabled` is false.
     *
     * \param renderer the renderer to render with
     */
    void render(const sdl::irenderer &renderer) const noexcept;

  private:
    std::deque<std::unique_ptr<iscene>> m_scenes; // A deque of scenes, index 0 = front.
};

} // namespace ccsakura
