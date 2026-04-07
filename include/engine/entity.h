/**
 * \file engine/entity.h
 *
 * Provides the entity system for the engine.
 */

#pragma once

#include "component.h"

#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace ccsakura
{

/**
 * Represents an abstract entity.
 */
class entity
{
  public:
    /**
     * Constructs a new entity with an ID.
     *
     * \param id the entity id
     */
    entity(uint32_t id);

    /**
     * Returns the entity's ID.
     */
    uint32_t id() const noexcept;

    /**
     * Checks if this entity has a component.
     *
     * \return true if the entity has the provided component
     */
    template <typename T>
        requires(std::is_base_of_v<component, T>)
    bool has_component()
    {
        return m_components.contains(std::type_index(typeid(T)));
    }

    /**
     * Creates and adds a certain component to the entity.
     *
     * \param args the argument to pass to the component constructor
     */
    template <typename T, typename... Args>
        requires(std::is_base_of_v<component, T>)
    void add_component(Args &&...args)
    {
        std::unique_ptr<T> comp = std::make_unique<T>(std::forward<Args>(args)...);
        m_components[std::type_index(typeid(T))] = std::move(comp);
    }

    template <typename T>
        requires(std::is_base_of_v<component, T>)
    T &get_component()
    {
        return static_cast<T &>(*m_components.at(std::type_index(typeid(T))));
    }

  private:
    std::unordered_map<std::type_index, std::unique_ptr<component>> m_components;
    uint32_t m_id;
};

/**
 * Fluent builder for constructing entities with components.
 *
 * Usage: entity_builder(id).with_component<T>(args...).edit_component<T>(fn).build()
 */
class entity_builder
{
  public:
    /**
     * Constructs a builder for an entity with the given ID.
     *
     * \param id the entity ID
     */
    entity_builder(uint32_t id);

    /**
     * Adds a component to the entity under construction.
     *
     * \tparam T the component type
     * \param args arguments forwarded to the component constructor
     * \returns this builder for chaining
     */
    template <typename T, typename... Args>
        requires(std::is_base_of_v<component, T>)
    entity_builder &with_component(Args &&...args)
    {
        m_entity.add_component<T>(std::forward<Args>(args)...);
        return *this;
    }

    /**
     * Applies a function to an already-added component, allowing mutation before build.
     *
     * \tparam T the component type — must have been added via with_component first
     * \param fn callable receiving a \c T& reference
     * \returns this builder for chaining
     */
    template <typename T, typename Fn>
        requires(std::is_base_of_v<component, T>)
    entity_builder &edit_component(Fn &&fn)
    {
        fn(m_entity.get_component<T>());
        return *this;
    }

    /**
     * Finalises and returns the constructed entity.
     *
     * \returns the built entity (ownership transferred to caller)
     */
    entity build();

  private:
    entity m_entity;
};

} // namespace ccsakura
