#include "sdl/sdl_rect.h"

#include "utils.h"

#include <SDL3/SDL_rect.h>

namespace sdl
{

point::point() noexcept : x(0), y(0)
{
}

point::point(const int x, const int y) noexcept : x(x), y(y)
{
}

point::point(const fpoint &other) noexcept
{
    x = static_cast<int>(other.x);
    y = static_cast<int>(other.y);
}

bool point::operator==(const point &other) const noexcept
{
    return x == other.x && y == other.y;
}

bool point::operator!=(const point &other) const noexcept
{
    return x != other.x || y != other.y;
}

SDL_Point point::to_sdl() const noexcept
{
    SDL_Point p = {x, y};
    return p;
}

fpoint::fpoint() noexcept : x(0), y(0)
{
}

fpoint::fpoint(const float x, const float y) noexcept : x(x), y(y)
{
}

fpoint::fpoint(const point &other) noexcept : x(static_cast<float>(other.x)), y(static_cast<float>(other.y))
{
}

bool fpoint::operator==(const fpoint &other) const noexcept
{
    return float_equal(x, other.x) && float_equal(y, other.y);
}

bool fpoint::operator!=(const fpoint &other) const noexcept
{
    return !float_equal(x, other.x) || !float_equal(y, other.y);
}

SDL_FPoint fpoint::to_sdl() const noexcept
{
    SDL_FPoint p = {x, y};
    return p;
}

// =================================

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

bool rect::operator==(const rect &other) const noexcept
{
    return x == other.x && y == other.y && w == other.w && h == other.h;
}

bool rect::operator!=(const rect &other) const noexcept
{
    return x != other.x || y != other.y || w != other.w || h != other.h;
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

bool frect::operator==(const frect &other) const noexcept
{
    return float_equal(x, other.x) && float_equal(y, other.y) && float_equal(w, other.w) && float_equal(h, other.h);
}

bool frect::operator!=(const frect &other) const noexcept
{
    return !float_equal(x, other.x) || !float_equal(y, other.y) || !float_equal(w, other.w) || !float_equal(h, other.h);
}

} // namespace sdl
