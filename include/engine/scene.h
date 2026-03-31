/**
 * \file engine/scene.h
 *
 * Provides a scene-based approach to rendering layers for the engine.
 */

#pragma once

#include "scenes/scene_id.h"
#include "sdl/sdl_render.h"
#include "signal.h"

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
     * \brief Called when the scene becomes the top-most active scene.
     */
    virtual void on_enter();

    /**
     * \brief Called when the scene is no longer the top-most active scene.
     */
    virtual void on_exit();

    /**
     * \brief Logic update phase. Called every frame with variable time step.
     * \param dt Delta time in seconds.
     */
    virtual void on_tick(const double dt) noexcept;

    /**
     * \brief Physical update phase. Called once every fixed time step (approx. 16.6ms).
     */
    virtual void on_physical_tick() noexcept;

    /**
     * \brief Signal/Event processing phase.
     * \param signal The signal to process.
     * \return true if the signal was consumed and should stop propagating, false otherwise.
     */
    virtual bool on_signal(isignal &signal) noexcept;

    /**
     * \brief Rendering phase.
     */
    virtual void on_render(const sdl::irenderer &renderer) const noexcept;

    /**
     * \brief Whether the scene blocks signals/events from reaching scenes below it.
     *
     * Example: A pause menu should return true; a HUD should return false.
     *
     * \return true if modal, false otherwise.
     */
    virtual bool is_modal() const noexcept;

    /**
     * \brief Whether the scene blocks rendering of scenes below it.
     *
     * Example: A full-screen menu should return true; a HUD should return false.
     *
     * \return true if opaque, false otherwise.
     */
    virtual bool is_opaque() const noexcept;
};

} // namespace ccsakura
