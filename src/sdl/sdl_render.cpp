#include "sdl/sdl_render.h"

#include "sdl/sdl_log.h"
#include "sdl/sdl_rect.h"
#include "sdl/sdl_surface.h"

#include <algorithm>
#include <memory>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

namespace sdl
{

vertex::vertex(const fpoint pos, const fcolor color) : pos(pos), color(color)
{
}

vertex::vertex(const fpoint pos, const fcolor color, const fpoint tex_coord)
    : pos(pos), color(color), tex_coord(tex_coord)
{
}

bool vertex::operator==(const vertex &other) const noexcept
{
    return pos == other.pos && tex_coord == other.tex_coord && color == other.color;
}

bool vertex::operator!=(const vertex &other) const noexcept
{
    return pos != other.pos || tex_coord != other.tex_coord || color != other.color;
}

SDL_Vertex vertex::to_sdl() const noexcept
{
    SDL_Vertex vertex;
    vertex.color = {color.r, color.g, color.b, color.a};
    vertex.position = {pos.x, pos.y};
    vertex.tex_coord = {tex_coord.x, tex_coord.y};
    return vertex;
}

// =============================================

render_geometry_options::render_geometry_options(const irenderer &renderer) : renderer(renderer)
{
}

render_geometry_options &render_geometry_options::use_texture(sdl::itexture &txt) noexcept
{
    texture = &txt;
    return *this;
}

render_geometry_options &render_geometry_options::add_vertex(const sdl::vertex &vertex) noexcept
{
    vertices.push_back(vertex);
    return *this;
}

render_geometry_options &render_geometry_options::connect(const int a, const int b, const int c) noexcept
{
    indices.push_back(a);
    indices.push_back(b);
    indices.push_back(c);
    return *this;
}

void render_geometry_options::render() const noexcept
{
    renderer.render_geometry(*this);
}

// =============================================

renderer::renderer(const iwindow &window, const char *name)
    : m_renderer(SDL_CreateRenderer(window.get(), name), SDL_DestroyRenderer)
{
    if (!m_renderer)
    {
        sdl::log_critical("Unable to setup SDL renderer: {}", SDL_GetError());
        throw std::runtime_error("Unable to setup SDL renderer");
    }

    sdl::log_trace("sdl::renderer constructed with window {} named {}", SDL_GetWindowTitle(window.get()),
                   SDL_GetRendererName(m_renderer.get()));
    SDL_SetRenderVSync(m_renderer.get(), 1);
    m_name = SDL_GetRendererName(m_renderer.get());
}

SDL_Renderer *renderer::get() const noexcept
{
    return m_renderer.get();
}

blend_mode renderer::get_blend_mode() const noexcept
{
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(get(), &mode);
    return static_cast<blend_mode>(mode);
}

void renderer::set_blend_mode(const blend_mode mode) const noexcept
{
    SDL_SetRenderDrawBlendMode(get(), static_cast<SDL_BlendMode>(mode));
}

void renderer::set_logical_presentation(const int w, const int h, const logical_presentation mode) const noexcept
{
    sdl::log_trace("sdl::renderer {} setting logical presentation to {}x{} mode {}", m_name, w, h,
                   static_cast<int>(mode));
    if (!SDL_SetRenderLogicalPresentation(m_renderer.get(), w, h, static_cast<SDL_RendererLogicalPresentation>(mode)))
    {
        sdl::log_error("Failed to set logical presentation: {}", SDL_GetError());
    }
}

std::unique_ptr<itexture> renderer::create_texture(const pixel_format format, const texture_access access, const int w,
                                                   const int h) const
{
    sdl::log_verbose("sdl::renderer {} creating a texture {}x{}", m_name, w, h);
    SDL_Texture *txt = SDL_CreateTexture(m_renderer.get(), static_cast<SDL_PixelFormat>(format),
                                         static_cast<SDL_TextureAccess>(access), w, h);

    if (!txt)
    {
        sdl::log_error("Unable to create a texture: {}", SDL_GetError());
        throw std::runtime_error("Unable to create a texture from format and access");
    }

    auto texture_ptr = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)>(txt, SDL_DestroyTexture);
    return std::make_unique<texture>(std::move(texture_ptr));
}

std::unique_ptr<itexture> renderer::create_texture(const sdl::isurface &surface) const
{
    SDL_Texture *txt = SDL_CreateTextureFromSurface(m_renderer.get(), surface.get());
    if (!txt)
    {
        sdl::log_error("Unable to create texture from surface: {}", SDL_GetError());
        throw std::runtime_error("Unable to create texture from surface");
    }

    auto txt_ptr = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)>(txt, SDL_DestroyTexture);
    return std::make_unique<texture>(std::move(txt_ptr));
}

void renderer::render_texture(sdl::itexture &texture, const sdl::frect *srcrect,
                              const sdl::frect *dstrect) const noexcept
{
    SDL_FRect sdl_src, sdl_dst;
    const SDL_FRect *src_ptr = nullptr;
    const SDL_FRect *dst_ptr = nullptr;
    if (srcrect)
    {
        sdl_src = {srcrect->x, srcrect->y, srcrect->w, srcrect->h};
        src_ptr = &sdl_src;
    }
    if (dstrect)
    {
        sdl_dst = {dstrect->x, dstrect->y, dstrect->w, dstrect->h};
        dst_ptr = &sdl_dst;
    }
    SDL_RenderTexture(m_renderer.get(), texture.get(), src_ptr, dst_ptr);
}

void renderer::render_texture_rotated(sdl::itexture &texture, const sdl::frect *srcrect, const sdl::frect *dstrect,
                                      double angle, const sdl::fpoint *center, sdl::flip flip_mode) const noexcept
{
    SDL_FRect sdl_src, sdl_dst;
    SDL_FPoint sdl_center;
    const SDL_FRect *src_ptr = nullptr;
    const SDL_FRect *dst_ptr = nullptr;
    const SDL_FPoint *center_ptr = nullptr;
    if (srcrect)
    {
        sdl_src = {srcrect->x, srcrect->y, srcrect->w, srcrect->h};
        src_ptr = &sdl_src;
    }
    if (dstrect)
    {
        sdl_dst = {dstrect->x, dstrect->y, dstrect->w, dstrect->h};
        dst_ptr = &sdl_dst;
    }
    if (center)
    {
        sdl_center = {center->x, center->y};
        center_ptr = &sdl_center;
    }
    SDL_RenderTextureRotated(m_renderer.get(), texture.get(), src_ptr, dst_ptr, angle, center_ptr,
                             static_cast<SDL_FlipMode>(flip_mode));
}

void renderer::render_texture_affine(sdl::itexture &texture, const sdl::frect *srcrect, const sdl::fpoint *origin,
                                     const sdl::fpoint *right, const sdl::fpoint *down) const noexcept
{
    SDL_FRect sdl_src;
    SDL_FPoint sdl_origin, sdl_right, sdl_down;
    const SDL_FRect *src_ptr = nullptr;
    const SDL_FPoint *origin_ptr = nullptr;
    const SDL_FPoint *right_ptr = nullptr;
    const SDL_FPoint *down_ptr = nullptr;
    if (srcrect)
    {
        sdl_src = {srcrect->x, srcrect->y, srcrect->w, srcrect->h};
        src_ptr = &sdl_src;
    }
    if (origin)
    {
        sdl_origin = {origin->x, origin->y};
        origin_ptr = &sdl_origin;
    }
    if (right)
    {
        sdl_right = {right->x, right->y};
        right_ptr = &sdl_right;
    }
    if (down)
    {
        sdl_down = {down->x, down->y};
        down_ptr = &sdl_down;
    }
    SDL_RenderTextureAffine(m_renderer.get(), texture.get(), src_ptr, origin_ptr, right_ptr, down_ptr);
}

void renderer::render_fill_rect(const sdl::frect &rect) const noexcept
{
    sdl::log_verbose("sdl::renderer {} render_rect", m_name);
    SDL_FRect r = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(m_renderer.get(), &r);
}

void renderer::render_geometry(const render_geometry_options &opts) const noexcept
{
    SDL_Texture *txt = nullptr;
    if (opts.texture)
    {
        txt = opts.texture->get();
    }

    std::vector<SDL_Vertex> vertices;
    vertices.reserve(opts.vertices.size());
    std::transform(opts.vertices.begin(), opts.vertices.end(), std::back_inserter(vertices),
                   [&](const sdl::vertex &v) { return v.to_sdl(); });

    SDL_RenderGeometry(opts.renderer.get(), txt, vertices.data(), static_cast<int>(vertices.size()),
                       opts.indices.data(), static_cast<int>(opts.indices.size()));
}

void renderer::set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const noexcept
{
    sdl::log_trace("sdl::renderer {} color set to {}, {}, {}, {}", m_name, r, g, b, a);
    SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a);
}

void renderer::set_color(const float r, const float g, const float b, const float a) const noexcept
{
    sdl::log_trace("sdl::renderer {} color set to {}, {}, {}, {}", m_name, r, g, b, a);
    SDL_SetRenderDrawColorFloat(m_renderer.get(), r, g, b, a);
}

sdl::fpoint renderer::coordinates_from_window(float x, float y) const noexcept
{
    float lx, ly;
    SDL_RenderCoordinatesFromWindow(m_renderer.get(), x, y, &lx, &ly);
    return {lx, ly};
}

void renderer::clear() const noexcept
{
    sdl::log_trace("sdl::renderer {} clear", m_name);
    SDL_RenderClear(m_renderer.get());
}

void renderer::present() const noexcept
{
    sdl::log_trace("sdl::renderer {} present", m_name);
    SDL_RenderPresent(m_renderer.get());
}

void renderer::set_render_target(sdl::itexture *texture) const noexcept
{
    SDL_SetRenderTarget(m_renderer.get(), texture ? texture->get() : nullptr);
}

renderer::~renderer()
{
    sdl::log_trace("sdl::renderer destroyed");
}

} // namespace sdl
