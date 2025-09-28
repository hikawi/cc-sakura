#include "engine/vec2d.h"
#include "engine/vec3d.h"

#include <ostream>

namespace ccsakura
{

void PrintTo(const ccsakura::vec2d &vec, std::ostream *os)
{
    *os << "(" << vec.x << "," << vec.y << ")";
}

void PrintTo(const ccsakura::vec3d &vec, std::ostream *os)
{
    *os << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}

} // namespace ccsakura
