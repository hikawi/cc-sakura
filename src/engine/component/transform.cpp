#include "engine/component/transform.h"

namespace ccsakura::components
{

transform::transform(vec2d pos, double rot, bool fixed) : position(pos), rotation(rot), fixed(fixed)
{
}

} // namespace ccsakura::components
