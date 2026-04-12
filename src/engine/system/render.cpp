#include "engine/system/render.h"

#include "engine/component/collider.h"
#include "engine/component/transform.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <variant>

namespace ccsakura::system
{

void render_aabb_collider(const components::aabb_collider &collider, const sdl::fcolor &color,
                          const sdl::irenderer &renderer, const camera2d &camera)
{
    renderer.set_color(color.r, color.g, color.b, color.a);
    const sdl::fpoint sc = world_to_screen(collider.center, camera);
    const float sw = static_cast<float>(collider.half_extents.x * 2.0 * camera.zoom);
    const float sh = static_cast<float>(collider.half_extents.y * 2.0 * camera.zoom);
    renderer.render_fill_rect(sdl::frect(sc.x - sw / 2, sc.y - sh / 2, sw, sh));
}

void render_obb_collider(const components::obb_collider &collider, const sdl::fcolor &color,
                         const sdl::irenderer &renderer, const camera2d &camera)
{
    renderer.set_color(color.r, color.g, color.b, color.a);

    double cos_a = std::cos(collider.angle);
    double sin_a = std::sin(collider.angle);
    vec2d ux(cos_a, sin_a);
    vec2d uy(-sin_a, cos_a);

    std::array<vec2d, 4> corners = {
        collider.center - ux * collider.half_extents.x - uy * collider.half_extents.y, // bottom-left
        collider.center + ux * collider.half_extents.x - uy * collider.half_extents.y, // bottom-right
        collider.center + ux * collider.half_extents.x + uy * collider.half_extents.y, // top-right
        collider.center - ux * collider.half_extents.x + uy * collider.half_extents.y  // top-left
    };

    sdl::render_geometry_options opts(renderer);
    for (const auto &c : corners)
    {
        opts.add_vertex(sdl::vertex(world_to_screen(c, camera), color));
    }

    opts.connect(0, 1, 2);
    opts.connect(0, 2, 3);
    opts.render();
}

void render_circle_collider(const components::circle_collider &collider, const sdl::fcolor &color,
                            const sdl::irenderer &renderer, const camera2d &camera)
{
    renderer.set_color(color.r, color.g, color.b, color.a);

    const sdl::fpoint sc = world_to_screen(collider.center, camera);
    const double sr = collider.radius * camera.zoom;

    sdl::render_geometry_options opts(renderer);
    opts.add_vertex(sdl::vertex(sc, color));

    constexpr int segments = 64;
    for (int i = 0; i <= segments; ++i)
    {
        double angle = 2.0 * std::numbers::pi * i / segments;
        sdl::fpoint p(sc.x + static_cast<float>(std::cos(angle) * sr), sc.y + static_cast<float>(std::sin(angle) * sr));
        opts.add_vertex(sdl::vertex(p, color));
    }

    for (int i = 1; i <= segments; ++i)
    {
        opts.connect(0, i, i + 1);
    }

    opts.render();
}

void render_capsule_collider(const components::capsule_collider &collider, const sdl::fcolor &color,
                             const sdl::irenderer &renderer, const camera2d &camera)
{
    // If it's just a circle, render it as such.
    if (collider.p1 == collider.p2)
    {
        render_circle_collider({collider.p1, collider.radius}, color, renderer, camera);
        return;
    }

    renderer.set_color(color.r, color.g, color.b, color.a);
    vec2d d = collider.p2 - collider.p1;
    vec2d norm = d.orthogonal().normalized() * collider.radius;

    // Rectangle corners (world-space, then transformed to screen)
    const sdl::fpoint sr1 = world_to_screen(collider.p1 + norm, camera);
    const sdl::fpoint sr2 = world_to_screen(collider.p2 + norm, camera);
    const sdl::fpoint sr3 = world_to_screen(collider.p2 - norm, camera);
    const sdl::fpoint sr4 = world_to_screen(collider.p1 - norm, camera);

    const sdl::fpoint sp1 = world_to_screen(collider.p1, camera);
    const sdl::fpoint sp2 = world_to_screen(collider.p2, camera);
    const double sr = collider.radius * camera.zoom;

    sdl::render_geometry_options opts(renderer);

    // Add rectangle vertices
    int rect_start = static_cast<int>(opts.vertices.size());
    opts.add_vertex(sdl::vertex(sr1, color));
    opts.add_vertex(sdl::vertex(sr2, color));
    opts.add_vertex(sdl::vertex(sr3, color));
    opts.add_vertex(sdl::vertex(sr4, color));

    opts.connect(rect_start + 0, rect_start + 1, rect_start + 2);
    opts.connect(rect_start + 0, rect_start + 2, rect_start + 3);

    // Semi-circles at endpoints
    constexpr int segments = 32;
    double base_angle = std::atan2(norm.y, norm.x);

    // Semi-circle at p1 (around p1, from r4 to r1, away from p2)
    int p1_center_idx = static_cast<int>(opts.vertices.size());
    opts.add_vertex(sdl::vertex(sp1, color));
    for (int i = 0; i <= segments; ++i)
    {
        double angle = base_angle + std::numbers::pi * i / segments;
        sdl::fpoint p(sp1.x + static_cast<float>(std::cos(angle) * sr),
                      sp1.y + static_cast<float>(std::sin(angle) * sr));
        opts.add_vertex(sdl::vertex(p, color));
    }
    for (int i = 1; i <= segments; ++i)
    {
        opts.connect(p1_center_idx, p1_center_idx + i, p1_center_idx + i + 1);
    }

    // Semi-circle at p2 (around p2, from r2 to r3, away from p1)
    int p2_center_idx = static_cast<int>(opts.vertices.size());
    opts.add_vertex(sdl::vertex(sp2, color));
    for (int i = 0; i <= segments; ++i)
    {
        double angle = base_angle + std::numbers::pi + std::numbers::pi * i / segments;
        sdl::fpoint p(sp2.x + static_cast<float>(std::cos(angle) * sr),
                      sp2.y + static_cast<float>(std::sin(angle) * sr));
        opts.add_vertex(sdl::vertex(p, color));
    }
    for (int i = 1; i <= segments; ++i)
    {
        opts.connect(p2_center_idx, p2_center_idx + i, p2_center_idx + i + 1);
    }

    opts.render();
}

void render_collider(const components::collider &collider, const sdl::irenderer &renderer, const camera2d &camera)
{
    std::visit(overloaded{[&renderer, &camera, &collider](const components::aabb_collider &aabb)
                          { render_aabb_collider(aabb, collider.color, renderer, camera); },
                          [&renderer, &camera, &collider](const components::obb_collider &obb)
                          { render_obb_collider(obb, collider.color, renderer, camera); },
                          [&renderer, &camera, &collider](const components::circle_collider &circle)
                          { render_circle_collider(circle, collider.color, renderer, camera); },
                          [&renderer, &camera, &collider](const components::capsule_collider &capsule)
                          { render_capsule_collider(capsule, collider.color, renderer, camera); },
                          [](const auto &) { sdl::log_critical("Unknown collider type, can't be rendered"); }},
               collider.shape);
}

void render_sprite(const sdl::irenderer &renderer, const components::sprite &spr, const sdl::fpoint &screen_pos,
                   double rotation) noexcept
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

void render_text(const sdl::irenderer &renderer, const components::text &txt, const sdl::fpoint &screen_pos) noexcept
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
    const auto hb = ent.get_component<components::collider>();
    const auto txt = ent.get_component<components::text>();

    if (!spr && !hb && !txt)
        return;

    const auto transform = ent.get_component<components::transform>();
    sdl::fpoint screen_pos{0.0f, 0.0f};
    if (transform)
    {
        screen_pos = transform->fixed ? transform->position.to_fpoint() : world_to_screen(transform->position, cam);
    }
    else
    {
        sdl::log_warn("Entity {} has no transform component.", ent.id());
    }

    if (spr)
    {
        render_sprite(renderer, *spr, screen_pos, transform ? transform->rotation : 0.0);
    }

    if (hb)
    {
        render_collider(*hb, renderer, cam);
    }

    if (txt)
    {
        render_text(renderer, *txt, screen_pos);
    }
}

} // namespace ccsakura::system
