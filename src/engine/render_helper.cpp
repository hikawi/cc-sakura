#include "engine/render_helper.h"

#include "sdl/sdl_log.h"

namespace ccsakura
{

render_texture_options::render_texture_options(const sdl::irenderer &renderer, sdl::itexture &texture)
    : m_renderer(renderer), m_texture(texture)
{
}

render_texture_options &render_texture_options::srcrect(const sdl::frect rect) noexcept
{
    m_srcrect = rect;
    return *this;
}

render_texture_options &render_texture_options::dstrect(const sdl::frect rect) noexcept
{
    m_dstrect = rect;
    return *this;
}

render_texture_options &render_texture_options::dst(const sdl::fpoint pos) noexcept
{
    float w = 0, h = 0;

    if (m_srcrect.has_value())
    {
        w = m_srcrect->w;
        h = m_srcrect->h;
    }
    else
    {
        sdl::log_error("dst called before srcrect is set: {}, {}", pos.x, pos.y);
        w = static_cast<float>(m_texture.get()->w);
        h = static_cast<float>(m_texture.get()->h);
    }

    m_dstrect = {pos.x, pos.y, w, h};
    return *this;
}

render_texture_options &render_texture_options::render_origin(const ccsakura::render_origin origin) noexcept
{
    m_render_origin = origin;
    return *this;
}

render_texture_options &render_texture_options::origin(const sdl::fpoint pos) noexcept
{
    m_origin = pos;
    return *this;
}

render_texture_options &render_texture_options::rotate(const double angle) noexcept
{
    m_rotation = angle;
    return *this;
}

render_texture_options &render_texture_options::flip(const sdl::flip flipmode) noexcept
{
    m_flip_mode = flipmode;
    return *this;
}

render_texture_options &render_texture_options::color_mod(const sdl::fcolor c) noexcept
{
    m_color_mod = c;
    return *this;
}

void render_texture_options::render() const noexcept
{
    sdl::frect dstrect = m_dstrect.value_or(sdl::frect{});
    shift_origin(dstrect, m_render_origin);

    std::optional<sdl::fpoint> center;
    if (m_origin)
    {
        sdl::frect center_rect{m_origin->x, m_origin->y, dstrect.w, dstrect.h};
        shift_origin(center_rect, m_render_origin);
        center = sdl::fpoint{center_rect.x, center_rect.y};
    }

    if (m_color_mod)
        m_texture.set_color_mod(m_color_mod->r, m_color_mod->g, m_color_mod->b);

    const sdl::frect *srcrect_ptr = m_srcrect ? &*m_srcrect : nullptr;
    const sdl::frect *dstrect_ptr = m_dstrect ? &dstrect : nullptr;
    const sdl::fpoint *center_ptr = center ? &*center : nullptr;

    m_renderer.render_texture_rotated(m_texture, srcrect_ptr, dstrect_ptr, m_rotation, center_ptr, m_flip_mode);

    if (m_color_mod)
        m_texture.set_color_mod(1.0f, 1.0f, 1.0f);
}

} // namespace ccsakura
