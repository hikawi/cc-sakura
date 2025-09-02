#include "engine/vec2d.h"

#include "utils.h"

#include <cmath>

namespace ccsakura
{

bool vec2d::operator==(const vec2d &rhs) const noexcept
{
    return double_equal(x, rhs.x) && double_equal(y, rhs.y);
}

bool vec2d::operator!=(const vec2d &rhs) const noexcept
{
    return !double_equal(x, rhs.x) || !double_equal(y, rhs.y);
}

[[nodiscard]] double vec2d::length() const noexcept
{
    return std::sqrt(length_squared());
}

[[nodiscard]] vec2d vec2d::normalized() const noexcept
{
    const double len = length();
    return len > 0 ? *this / len : *this;
}

[[nodiscard]] double vec2d::distance(const vec2d &rhs) const noexcept
{
    return (rhs - *this).length();
}

} // namespace ccsakura
