/**
 * \file engine/engine.h
 *
 * As the name implies, the main engine coordinator of the system. Everything else should be delegated from here.
 */

#pragma once

#include "app.h"

#include <cstdint>
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
 * Abstracted interface of \ref ccsakura::engine for mocking purposes.
 */
class iengine
{
  public:
    /**
     * Maximum duration allowed for a variable time step.
     */
    static constexpr const double s_max_dt = 0.1;

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
     */
    engine(std::unique_ptr<ccsakura::iapp> app);
    ~engine() noexcept override;

    struct frame_data get_frame_data() const override;
    bool iterate(const uint64_t tick) noexcept override;
    void render() const noexcept override;

  private:
    std::unique_ptr<iapp> m_app;
    struct frame_data m_frame_data;
};

} // namespace ccsakura
