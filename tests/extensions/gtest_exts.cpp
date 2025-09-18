#include "engine/vec2d.h"

#include <ostream>

namespace ccsakura
{

void PrintTo(const ccsakura::vec2d &vec, std::ostream *os)
{
    *os << "(" << vec.x << "," << vec.y << ")";
}

} // namespace ccsakura
