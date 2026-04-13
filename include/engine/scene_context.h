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
     * \returns the handle assigned to the scene.
     */
    virtual scene_handle push_back(std::unique_ptr<iscene> scene) = 0;

    /**
     * \brief Queues a request to push a scene to the front of the stack.
     * \param scene The scene to add.
     * \returns the handle assigned to the scene.
     */
    virtual scene_handle push_front(std::unique_ptr<iscene> scene) = 0;

    /**
     * \brief Queues a request to push a scene before the first scene found of that type.
     *
     * If there is no scene with that type, acts the same as `push_front`.
     *
     * \param scene the scene to add.
     * \param type the category of scene to push before.
     * \returns the handle assigned to the scene.
     */
    virtual scene_handle push_before(std::unique_ptr<iscene> scene, const scene_type type) = 0;

    /**
     * \brief Queues a request to push a scene right after the last scene found of that type.
     *
     * If there is no scene with that type, acts the same as `push_back`.
     *
     * \param scene the scene to add.
     * \param type the category of scene to push after.
     * \returns the handle assigned to the scene.
     */
    virtual scene_handle push_after(std::unique_ptr<iscene> scene, const scene_type type) = 0;

    /**
     * \brief Queues a request to remove the front scene from the stack.
     *
     * If the stack is empty at processing time, the request is ignored.
     */
    virtual void pop_front() = 0;

    /**
     * \brief Queues a request to remove the last scene from the stack.
     *
     * If the stack is empty at processing time, the request is ignored.
     */
    virtual void pop_last() = 0;

    /**
     * \brief Queues a request to remove the first scene of the given category.
     *
     * If no scene with \p type is found at processing time, the request is ignored.
     *
     * \param type the category of scene to remove.
     */
    virtual void pop_of_type(const scene_type type) = 0;

    /**
     * \brief Queues a request to replace a scene in-place with a transition effect.
     *
     * The incoming scene is inserted at the same stack depth as \p from. Other
     * scenes (e.g. HUD) remain untouched at their positions. The outgoing scene
     * is removed once the transition completes.
     *
     * If no scene with \p from is found in the stack, the request is ignored.
     * If the scene identified by \p from is already part of an active transition,
     * the request is ignored.
     *
     * \param to_scene   The incoming scene.
     * \param from       Handle of the outgoing scene to replace.
     * \param transition The transition effect and duration.
     * \returns the handle assigned to to_scene, or invalid_scene_handle if ignored.
     */
    virtual scene_handle start_transition(std::unique_ptr<iscene> to_scene, scene_handle from,
                                          scene_transition transition) = 0;

    /**
     * \brief Queues a request to remove a specific scene instance by handle.
     *
     * Calls on_pause then on_detach on the scene. If no scene with \p handle
     * is found in the stack, the request is ignored.
     *
     * \param handle the handle of the scene to remove.
     */
    virtual void pop(scene_handle handle) = 0;

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
