#include "sdl/sdl_render.h"

#include "engine/render.h"
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

// ========================================

texture::texture(std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_ptr) : m_texture(std::move(texture_ptr))
{
    if (!m_texture)
    {
        sdl::log_critical("Unable to setup SDL texture: {}", SDL_GetError());
        throw std::runtime_error("Unable to setup SDL Texture");
    }

    sdl::log_trace("sdl::texture constructed");
}

texture::~texture()
{
    sdl::log_trace("sdl::texture destroyed");
}

SDL_Texture *texture::get() const noexcept
{
    return m_texture.get();
}

blend_mode texture::get_blend_mode() const noexcept
{
    SDL_BlendMode mode;
    SDL_GetTextureBlendMode(m_texture.get(), &mode);
    return static_cast<blend_mode>(mode);
}

void texture::set_blend_mode(const blend_mode mode) const noexcept
{
    if (!SDL_SetTextureBlendMode(m_texture.get(), static_cast<SDL_BlendMode>(mode)))
    {
        sdl::log_error("Failed to set texture blend mode: {}", SDL_GetError());
    }
}

scale_mode texture::get_scale_mode() const noexcept
{
    SDL_ScaleMode mode;
    SDL_GetTextureScaleMode(m_texture.get(), &mode);
    return static_cast<scale_mode>(mode);
}

void texture::set_scale_mode(const scale_mode mode) const noexcept
{
    if (!SDL_SetTextureScaleMode(m_texture.get(), static_cast<SDL_ScaleMode>(mode)))
    {
        sdl::log_error("Failed to set texture scale mode: {}", SDL_GetError());
    }
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

void renderer::render_texture(const render_texture_options &options) const noexcept
{
    auto ptr_or_null = [&](const auto &opt) -> auto * { return opt.has_value() ? &*opt : nullptr; };
    auto rect_shift = [&](sdl::frect rect) -> SDL_FRect
    {
        ccsakura::shift_origin(rect, options.m_render_origin);
        return {rect.x, rect.y, rect.w, rect.h};
    };
    auto point_shift = [&](const sdl::point &p) -> SDL_FPoint
    {
        sdl::frect dstrect = options.m_dstrect.value();
        sdl::frect center(static_cast<float>(p.x), static_cast<float>(p.y), dstrect.w, dstrect.h);
        ccsakura::shift_origin(center, options.m_render_origin);
        return {center.x, center.y};
    };

    std::optional<SDL_FRect> srcrect = options.m_srcrect.transform(rect_shift);
    std::optional<SDL_FRect> dstrect = options.m_dstrect.transform(rect_shift);
    std::optional<SDL_FPoint> center;
    if (dstrect && options.m_origin)
    {
        center = options.m_origin.transform(point_shift);
    }

    SDL_RenderTextureRotated(m_renderer.get(), options.m_texture.get(), ptr_or_null(srcrect), ptr_or_null(dstrect),
                             options.m_rotation, ptr_or_null(center), static_cast<SDL_FlipMode>(options.m_flip_mode));
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

renderer::~renderer()
{
    sdl::log_trace("sdl::renderer destroyed");
}

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

    w = static_cast<float>(m_texture.get()->w);
    h = static_cast<float>(m_texture.get()->h);

    if (m_srcrect.has_value())
    {
        sdl::frect rect = m_srcrect.value();
        w = rect.w;
        h = rect.h;
    }

    m_dstrect = {static_cast<float>(pos.x), static_cast<float>(pos.y), w, h};
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

void render_texture_options::render() const noexcept
{
    m_renderer.render_texture(*this);
}

} // namespace sdl
