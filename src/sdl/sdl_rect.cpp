#include "sdl/sdl_rect.h"

namespace sdl
{

rect::rect() noexcept : x(0), y(0), w(0), h(0)
{
}

rect::rect(const int x, const int y, const int w, const int h) noexcept : x(x), y(y), w(w), h(h)
{
}

rect::rect(const frect &frect) noexcept
{
    x = static_cast<int>(frect.x);
    y = static_cast<int>(frect.y);
    w = static_cast<int>(frect.w);
    h = static_cast<int>(frect.h);
}

frect::frect() noexcept : x(0), y(0), w(0), h(0)
{
}

frect::frect(const float x, const float y, const float w, const float h) noexcept : x(x), y(y), w(w), h(h)
{
}

frect::frect(const rect &rect) noexcept
{
    x = static_cast<float>(rect.x);
    y = static_cast<float>(rect.y);
    w = static_cast<float>(rect.w);
    h = static_cast<float>(rect.h);
}

} // namespace sdl
