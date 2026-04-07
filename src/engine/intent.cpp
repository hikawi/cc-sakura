/**
 * \file engine/intent.cpp
 *
 * Provides implementation for the intent system.
 */

#include "engine/intent.h"

namespace ccsakura
{

intent_binder::intent_binder(std::function<void(std::vector<std::pair<sdl::keycode, intent>>)> commit)
    : m_commit(std::move(commit))
{
}

intent_binder &intent_binder::map(sdl::keycode key, intent i)
{
    m_bindings.emplace_back(key, i);
    return *this;
}

void intent_binder::bind()
{
    m_commit(std::move(m_bindings));
}

} // namespace ccsakura
