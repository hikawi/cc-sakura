/**
 * \file engine/entity.cpp
 *
 * Provides implementation for the entity system.
 */

#include "engine/entity.h"

namespace ccsakura
{

entity::entity(uint32_t id) : m_id(id)
{
}

uint32_t entity::id() const noexcept
{
    return m_id;
}

} // namespace ccsakura
