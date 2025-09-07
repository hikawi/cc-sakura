#include "engine/render.h"

#include <utility>

namespace ccsakura
{

void shift_origin(sdl::frect &rect, const render_origin origin)
{
    switch (origin)
    {
    case render_origin::top_left: // Do nothing.
        break;
    case render_origin::top_center: // Move origin X to left by half.
        rect.x -= rect.w / 2;
        break;
    case render_origin::top_right: // Move origin X to left full.
        rect.x -= rect.w;
        break;
    case render_origin::middle_left: // Move origin Y down by half.
        rect.y -= rect.h / 2;
        break;
    case render_origin::middle_center: // Move both origins by half.
        rect.x -= rect.w / 2;
        rect.y -= rect.h / 2;
        break;
    case render_origin::middle_right: // Move X by full and Y by half.
        rect.x -= rect.w;
        rect.y -= rect.h / 2;
        break;
    case render_origin::bottom_left: // Move Y by full.
        rect.y -= rect.h;
        break;
    case render_origin::bottom_center: // Move Y full and X half.
        rect.x -= rect.w / 2;
        rect.y -= rect.h;
        break;
    case render_origin::bottom_right: // Move Y full and X full.
        rect.x -= rect.w;
        rect.y -= rect.h;
        break;
    }

    [[unlikely]];
    std::unreachable();
}

} // namespace ccsakura
