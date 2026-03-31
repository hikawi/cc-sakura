/**
 * \file engine/engine.h
 *
 * As the name implies, the main engine coordinator of the system. Everything else should be delegated from here.
 */

#pragma once

#include "app.h"
#include "engine/scene.h"
#include "engine/sprite.h"
#include "engine/text.h"
#include "sdl/sdl_events.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_video.h"

#include <cstdint>
#include <deque>
#include <memory>

namespace ccsakura
{

/**
 * Holds various information about the engine's frame data.
 */
struct frame_data
{
    uint64_t last_tick = 0; ///< last iteration time in ms
    double accumulator = 0; ///< frame's accumulator, in seconds
    double frame_time =
        0; ///< frame counter to isolate from accumulator, we count frames per SECOND, not frames per PHYSICAL TICK
    uint32_t cur_frames = 0; ///< the current second's frame count
    uint32_t fps = 0;        ///< the frames per second, updated every second
};

/**
 * A shopping list of all the dependencies needed for the engine to run.
 */
struct engine_deps
{
    std::unique_ptr<sdl::iwindow> m_window;        ///< the window of the application
    std::unique_ptr<sdl::irenderer> m_renderer;    ///< the renderer of the application
    std::unique_ptr<iapp> m_app;                   ///< the application itself
    std::unique_ptr<ifont_cache> m_font_cache;     ///< the font cache
    std::unique_ptr<isprite_cache> m_sprite_cache; ///< the sprite cache

    /**
     * Checks if the dependency struct is valid.
     *
     * It is only valid if all pointers passed in are non-null.
     *
     * \returns true if valid, false otherwise
     */
    bool is_valid() const noexcept;
};

/**
 * Abstracted interface of \ref ccsakura::engine for mocking purposes.
 */
class iengine
{
  public:
    /**
     * Maximum duration allowed for a variable time step.
     */
    static constexpr const double s_max_dt = 0.05;

    /**
     * Fixed time step for each physical call.
     */
    static constexpr const double s_time_step = 1 / 60.0;

    virtual ~iengine() = default;

    /**
     * Retrieves the engine's current frame data.
     *
     * \returns the current frame data
     */
    virtual frame_data get_frame_data() const = 0;

    /**
     * Iterates one frame of the engine.
     *
     * This iterates as many times as the machine allows per second, delegating to tick-based system. But, physical
     * ticks shall only be called once every 17ms or so.
     *
     * \warning This function's call stack should never throw an exception.
     * \param tick the tick in milliseconds ever since SDL is initialized
     * \returns true if the engine should continue, false otherwise
     */
    virtual bool iterate(const uint64_t tick) noexcept = 0;

    /**
     * Process the SDL event from the engine's point.
     *
     * \param event the event to parse.
     */
    virtual void queue_event(const sdl::event &&event) noexcept = 0;

    /**
     * Checks if the engine should be running this tick.
     *
     * \returns true to continue
     */
    virtual bool is_running() const noexcept = 0;

    /**
     * Renders the current state of the program.
     *
     * This iterates as many times as \ref engine::iterate is called.
     *
     * \warning This function's call stack should never throw an exception.
     */
    virtual void render() const noexcept = 0;
};

/**
 * The engine of the game.
 *
 * This is mainly a scene-based engine, using variable ticks (with deltatime) and physical ticks. This should provide
 * the abstraction towards SDL for the application.
 */
class engine : public iengine
{
  public:
    /**
     * Initializes the engine, and also the application.
     *
     * This utilizes the pattern Inversion of Control, apparently, but I don't know why. It just feels natural this way.
     * Throws an exception if app failed to initialize.
     *
     * \param deps the dependencies of the engine
     */
    engine(engine_deps &&deps);
    ~engine() noexcept override;

    frame_data get_frame_data() const override;
    bool iterate(const uint64_t tick) noexcept override;
    void queue_event(const sdl::event &&event) noexcept override;
    bool is_running() const noexcept override;
    void render() const noexcept override;

  private:
    engine_deps m_deps;
    frame_data m_frame_data;
    scene_manager m_scene_mgr;
    std::deque<sdl::event> m_events_queue;
    bool m_running = true;

    void process_events();
};

} // namespace ccsakura
