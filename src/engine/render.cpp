#include "engine/render.h"

#include "engine/component.h"
#include "engine/entity.h"
#include "engine/sprite.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"

#include <algorithm>
#include <cctype>
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

sdl::fpoint world_to_screen(const vec2d &world, const camera2d &cam) noexcept
{
    const vec2d screen = (world - cam.position) * cam.zoom + cam.viewport * 0.5;
    return screen.to_fpoint();
}

sdl::frect world_to_screen(const sdl::frect &world, const camera2d &cam) noexcept
{
    const sdl::fpoint top_left = world_to_screen(vec2d{double(world.x), double(world.y)}, cam);
    return {top_left.x, top_left.y, world.w * static_cast<float>(cam.zoom), world.h * static_cast<float>(cam.zoom)};
}

void render_sprite(const sdl::irenderer &renderer, const components::sprite &spr,
                   const sdl::fpoint &screen_pos, double rotation) noexcept
{
    if (!spr.spr)
        return;

    const auto frame = spr.spr->frame(spr.frame_index);
    spr.spr->render_options(renderer)
        .srcrect(sdl::frect(frame.frame))
        .dst(screen_pos)
        .rotate(rotation)
        .render_origin(spr.origin)
        .render();
}

void render_text(const sdl::irenderer &renderer, const components::text &txt,
                 const sdl::fpoint &screen_pos) noexcept
{
    if (txt.value.empty())
        return;

    auto &font = sprite::named("font");

    constexpr float SPACE_ADVANCE = 4.0f;
    constexpr float PUNCT_GAP = 2.0f;
    constexpr float CHAR_GAP = 1.0f;

    // First pass: measure total block size for origin shifting.
    float width = 0.0f, height = 0.0f;
    const size_t len = txt.value.size();
    for (size_t i = 0; i < len; i++)
    {
        const char c = txt.value[i];
        if (c < 32 || c > 126)
            continue;

        if (c == ' ')
        {
            if (i + 1 < len)
                width += SPACE_ADVANCE;
            continue;
        }

        const auto &frame = font.frame(static_cast<uint32_t>(c - 32));
        height = std::max(height, static_cast<float>(frame.frame.h));
        width += static_cast<float>(frame.frame.w);
        if (i + 1 < len)
            width += std::ispunct(static_cast<unsigned char>(c)) ? PUNCT_GAP : CHAR_GAP;
    }

    sdl::frect block{0.0f, 0.0f, width, height};
    shift_origin(block, txt.origin);

    float render_x = screen_pos.x + block.x;
    float render_y = screen_pos.y + block.y;

    // Second pass: render each glyph.
    for (std::size_t i = 0; i < len; ++i)
    {
        const char c = txt.value[i];
        if (c < 32 || c > 126)
            continue;

        if (c == ' ')
        {
            render_x += SPACE_ADVANCE;
            continue;
        }

        const auto &f = font.frame(static_cast<uint32_t>(c - 32));

        font.render_options(renderer)
            .srcrect(sdl::frect(f.frame))
            .dst(sdl::fpoint{render_x + static_cast<float>(f.spr_source_size.x),
                             render_y + static_cast<float>(f.spr_source_size.y)})
            .color_mod(txt.color)
            .render();

        const float gap = std::ispunct(static_cast<unsigned char>(c)) ? PUNCT_GAP : CHAR_GAP;
        render_x += static_cast<float>(f.frame.w) + gap;
    }
}

void render_entity(const sdl::irenderer &renderer, const entity &ent, const camera2d &cam) noexcept
{
    const auto spr = ent.get_component<components::sprite>();
    const auto hb = ent.get_component<components::hitbox>();
    const auto txt = ent.get_component<components::text>();

    if (!spr && !hb && !txt)
        return;

    const auto transform = ent.get_component<components::transform>();
    sdl::fpoint screen_pos{0.0f, 0.0f};
    if (transform)
    {
        screen_pos =
            transform->fixed ? transform->position.to_fpoint() : world_to_screen(transform->position, cam);
    }
    else
    {
        sdl::log_warn("Entity {} has no transform component.", ent.id());
    }

    if (spr)
        render_sprite(renderer, *spr, screen_pos, transform ? transform->rotation : 0.0);

    if (hb)
        hb->get().render(renderer, cam);

    if (txt)
        render_text(renderer, *txt, screen_pos);
}

} // namespace ccsakura
