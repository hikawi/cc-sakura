/**
 * \file engine/scene_context.h
 *
 * Provides the scene_context proxy interface for scenes to queue requests against the scene manager.
 */

#pragma once

#include "engine/scene_transition.h"
#include "engine/signal.h"
#include "scenes/scene_id.h"
#include "sdl/sdl_pixels.h"

#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>

namespace ccsakura
{

class iscene;

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
     * \brief Queues a request to replace a scene in-place with a transition effect.
     *
     * The incoming scene is inserted at the same stack depth as the scene of
     * \p from_type. Other scenes (e.g. HUD) remain untouched at their positions.
     * The outgoing scene is removed once the transition completes.
     *
     * If no scene with \p from_type is found, the request is ignored.
     * If a transition is already active, the request is ignored.
     *
     * \param to_scene  The incoming scene.
     * \param from_type The scene_type of the outgoing scene to replace.
     * \param transition The transition effect and duration.
     * \returns true if queued successfully, false otherwise.
     */
    virtual bool start_transition(std::unique_ptr<iscene> to_scene, scene_type from_type,
                                  scene_transition transition) = 0;

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

} // namespace ccsakura
