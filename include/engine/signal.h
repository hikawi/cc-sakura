/**
 * \file engine/signal.h
 *
 * Interface for signals that are passed down to scenes.
 */

#pragma once

#include <string_view>

namespace ccsakura
{

/**
 * \brief Base interface for all signals.
 */
class isignal
{
  public:
    virtual ~isignal() = default;

    /**
     * \brief Returns the unique type name of the signal.
     */
    virtual std::string_view type() const noexcept = 0;
};

namespace signals
{

}

} // namespace ccsakura
