#include "engine/render.h"

#include <utility>

namespace ccsakura
{

void shift_origin_top(sdl::frect &rect, const render_origin origin)
{
    switch (origin)
    {
    case render_origin::top_left: // Do nothing.
        return;
    case render_origin::top_center: // Move origin X to left by half.
        rect.x -= rect.w / 2;
        return;
    case render_origin::top_right: // Move origin X to left full.
        rect.x -= rect.w;
        return;
    default:
        // Nothing.
        break;
    }
}

void shift_origin_middle(sdl::frect &rect, const render_origin origin)
{
    rect.y -= rect.h / 2;
    switch (origin)
    {
    case render_origin::middle_left: // Move origin Y down by half.
        return;
    case render_origin::middle_center: // Move both origins by half.
        rect.x -= rect.w / 2;
        return;
    case render_origin::middle_right: // Move X by full and Y by half.
        rect.x -= rect.w;
        return;
    default:
        // Nothing.
        break;
    }
}

void shift_origin_bottom(sdl::frect &rect, const render_origin origin)
{
    rect.y -= rect.h;
    switch (origin)
    {
    case render_origin::bottom_left: // Move Y by full.
        return;
    case render_origin::bottom_center: // Move Y full and X half.
        rect.x -= rect.w / 2;
        return;
    case render_origin::bottom_right: // Move Y full and X full.
        rect.x -= rect.w;
        return;
    default:
        // Nothing.
        break;
    }
}

void shift_origin(sdl::frect &rect, const render_origin origin)
{
    switch (origin)
    {
    case render_origin::top_left:
    case render_origin::top_center:
    case render_origin::top_right:
        shift_origin_top(rect, origin);
        return;
    case render_origin::middle_left:
    case render_origin::middle_center:
    case render_origin::middle_right:
        shift_origin_middle(rect, origin);
        return;
    case render_origin::bottom_left:
    case render_origin::bottom_center:
    case render_origin::bottom_right:
        shift_origin_bottom(rect, origin);
        return;
    }

    [[unlikely]];
    std::unreachable();
}

} // namespace ccsakura
