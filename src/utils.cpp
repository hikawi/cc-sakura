#include "utils.h"

#include <cmath>

bool float_equal(const float a, const float b) noexcept
{
    return std::fabs(a - b) < static_cast<float>(epsilon);
}

bool double_equal(const double a, const double b) noexcept
{
    return std::fabs(a - b) < epsilon;
}
