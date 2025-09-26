#include "sdl/sdl_pixels.h"

#include "utils.h"

#include <algorithm>
#include <cmath>

namespace sdl
{

color::color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

color::color(const fcolor &color) noexcept
{
    r = static_cast<uint8_t>(std::lround(std::clamp(color.r, 0.0f, 1.0f) * 255));
    g = static_cast<uint8_t>(std::lround(std::clamp(color.g, 0.0f, 1.0f) * 255));
    b = static_cast<uint8_t>(std::lround(std::clamp(color.b, 0.0f, 1.0f) * 255));
    a = static_cast<uint8_t>(std::lround(std::clamp(color.a, 0.0f, 1.0f) * 255));
}

bool color::operator==(const color &other) const noexcept
{
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

fcolor::fcolor(const float r, const float g, const float b, const float a) noexcept
{
    this->r = std::clamp(r, 0.0f, 1.0f);
    this->g = std::clamp(g, 0.0f, 1.0f);
    this->b = std::clamp(b, 0.0f, 1.0f);
    this->a = std::clamp(a, 0.0f, 1.0f);
}

fcolor::fcolor(const color &color) noexcept
{
    r = static_cast<float>(color.r) / 255;
    g = static_cast<float>(color.g) / 255;
    b = static_cast<float>(color.b) / 255;
    a = static_cast<float>(color.a) / 255;
}

bool fcolor::operator==(const fcolor &other) const noexcept
{
    return float_equal(r, other.r) && float_equal(g, other.g) && float_equal(b, other.b) && float_equal(a, other.a);
}

} // namespace sdl
