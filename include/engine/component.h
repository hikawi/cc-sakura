/**
 * \file engine/component.h
 *
 * Provides the component abstraction for the engine.
 */

#pragma once

namespace ccsakura
{

/**
 * Represents a piece of abstraction that can be composed onto an entity.
 */
class component
{
  public:
    virtual ~component() = default;
};

} // namespace ccsakura
