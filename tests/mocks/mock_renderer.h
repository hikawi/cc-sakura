#pragma once

#include "sdl/sdl_render.h"
#include "sdl/sdl_surface.h"

#include "gmock/gmock.h"

class mock_renderer : public sdl::irenderer
{
  public:
    MOCK_METHOD(SDL_Renderer *, get, (), (const, noexcept, override));
    MOCK_METHOD(sdl::blend_mode, get_blend_mode, (), (const, noexcept, override));
    MOCK_METHOD(void, set_blend_mode, (const sdl::blend_mode), (const, noexcept, override));
    MOCK_METHOD(void, set_logical_presentation, (const int w, const int h, const sdl::logical_presentation mode),
                (const, noexcept, override));
    MOCK_METHOD(std::unique_ptr<sdl::itexture>, create_texture,
                (const sdl::pixel_format format, const sdl::texture_access access, const int w, const int h),
                (const, noexcept, override));
    MOCK_METHOD(std::unique_ptr<sdl::itexture>, create_texture, (const sdl::isurface &surface),
                (const, noexcept, override));
    MOCK_METHOD(void, render_texture, (const sdl::render_texture_options &), (const, noexcept, override));
    MOCK_METHOD(void, render_fill_rect, (const sdl::frect &rect), (const, noexcept, override));
    MOCK_METHOD(void, render_geometry, (const sdl::render_geometry_options &options), (const, noexcept, override));
    MOCK_METHOD(void, set_color, (const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a),
                (const, noexcept, override));
    MOCK_METHOD(void, set_color, (const float r, const float g, const float b, const float a),
                (const, noexcept, override));
    MOCK_METHOD(sdl::fpoint, coordinates_from_window, (float x, float y), (const, noexcept, override));
    MOCK_METHOD(void, clear, (), (const, noexcept, override));
    MOCK_METHOD(void, present, (), (const, noexcept, override));
    MOCK_METHOD(void, set_render_target, (sdl::itexture * texture), (const, noexcept, override));
};
