#include "engine/vec2d.h"
#include "engine/vec3d.h"

#include <ostream>

namespace ccsakura
{

void PrintTo(const ccsakura::vec2d &vec, std::ostream *os);

void PrintTo(const ccsakura::vec3d &vec, std::ostream *os);

} // namespace ccsakura
