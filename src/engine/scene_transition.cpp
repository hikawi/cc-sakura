#include "engine/scene_transition.h"

#include "engine/render_helper.h"

namespace ccsakura
{

namespace
{

void draw(const sdl::irenderer &r, sdl::itexture &tex, sdl::frect src, sdl::frect dst) noexcept
{
    render_texture_options(r, tex).srcrect(src).dstrect(dst).render();
}

void apply_fade(const sdl::irenderer &r, sdl::itexture &from, sdl::itexture &to, float t, float vw, float vh) noexcept
{
    from.set_alpha_mod(static_cast<uint8_t>(255.0f * (1.0f - t)));
    draw(r, from, {0, 0, vw, vh}, {0, 0, vw, vh});
    from.set_alpha_mod(255);

    to.set_alpha_mod(static_cast<uint8_t>(255.0f * t));
    draw(r, to, {0, 0, vw, vh}, {0, 0, vw, vh});
    to.set_alpha_mod(255);
}

void apply_slide_left(const sdl::irenderer &r, sdl::itexture &from, sdl::itexture &to, float t, float vw,
                      float vh) noexcept
{
    draw(r, from, {0, 0, vw, vh}, {-(vw * t), 0, vw, vh});
    draw(r, to, {0, 0, vw, vh}, {vw * (1.0f - t), 0, vw, vh});
}

void apply_slide_right(const sdl::irenderer &r, sdl::itexture &from, sdl::itexture &to, float t, float vw,
                       float vh) noexcept
{
    draw(r, from, {0, 0, vw, vh}, {vw * t, 0, vw, vh});
    draw(r, to, {0, 0, vw, vh}, {-(vw * (1.0f - t)), 0, vw, vh});
}

void apply_slide_up(const sdl::irenderer &r, sdl::itexture &from, sdl::itexture &to, float t, float vw,
                    float vh) noexcept
{
    draw(r, from, {0, 0, vw, vh}, {0, -(vh * t), vw, vh});
    draw(r, to, {0, 0, vw, vh}, {0, vh * (1.0f - t), vw, vh});
}

void apply_slide_down(const sdl::irenderer &r, sdl::itexture &from, sdl::itexture &to, float t, float vw,
                      float vh) noexcept
{
    draw(r, from, {0, 0, vw, vh}, {0, vh * t, vw, vh});
    draw(r, to, {0, 0, vw, vh}, {0, -(vh * (1.0f - t)), vw, vh});
}

void apply_split_horizontal(const sdl::irenderer &r, sdl::itexture &from, sdl::itexture &to, float t, float vw,
                            float vh) noexcept
{
    const float half = vh * 0.5f;
    draw(r, to, {0, 0, vw, vh}, {0, 0, vw, vh});
    draw(r, from, {0, 0, vw, half}, {0, -(half * t), vw, half});
    draw(r, from, {0, half, vw, half}, {0, half + (half * t), vw, half});
}

void apply_split_vertical(const sdl::irenderer &r, sdl::itexture &from, sdl::itexture &to, float t, float vw,
                          float vh) noexcept
{
    const float half = vw * 0.5f;
    draw(r, to, {0, 0, vw, vh}, {0, 0, vw, vh});
    draw(r, from, {0, 0, half, vh}, {-(half * t), 0, half, vh});
    draw(r, from, {half, 0, half, vh}, {half + (half * t), 0, half, vh});
}

} // namespace

void render_transition(const sdl::irenderer &renderer, sdl::itexture &from_tex, sdl::itexture &to_tex,
                       scene_transition_type type, double t, float vw, float vh) noexcept
{
    const float tf = static_cast<float>(t);
    switch (type)
    {
    case scene_transition_type::fade:
        apply_fade(renderer, from_tex, to_tex, tf, vw, vh);
        break;
    case scene_transition_type::slide_left:
        apply_slide_left(renderer, from_tex, to_tex, tf, vw, vh);
        break;
    case scene_transition_type::slide_right:
        apply_slide_right(renderer, from_tex, to_tex, tf, vw, vh);
        break;
    case scene_transition_type::slide_up:
        apply_slide_up(renderer, from_tex, to_tex, tf, vw, vh);
        break;
    case scene_transition_type::slide_down:
        apply_slide_down(renderer, from_tex, to_tex, tf, vw, vh);
        break;
    case scene_transition_type::split_horizontal:
        apply_split_horizontal(renderer, from_tex, to_tex, tf, vw, vh);
        break;
    case scene_transition_type::split_vertical:
        apply_split_vertical(renderer, from_tex, to_tex, tf, vw, vh);
        break;
    case scene_transition_type::none:
        render_texture_options(renderer, from_tex).dstrect({0.0f, 0.0f, vw, vh}).render();
        break;
    }
}

} // namespace ccsakura
