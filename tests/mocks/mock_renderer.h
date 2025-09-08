#pragma once

#include "sdl/sdl_render.h"

#include "gmock/gmock.h"

class mock_renderer : public sdl::irenderer
{
  public:
    MOCK_METHOD(SDL_Renderer *, get, (), (const, noexcept, override));
    MOCK_METHOD(std::unique_ptr<sdl::itexture>, create_texture,
                (sdl::pixel_format format, sdl::texture_access access, int w, int h), (const, noexcept, override));
    MOCK_METHOD(void, set_color, (const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a),
                (const, noexcept, override));
    MOCK_METHOD(void, set_color, (const float r, const float g, const float b, const float a),
                (const, noexcept, override));
    MOCK_METHOD(void, clear, (), (const, noexcept, override));
    MOCK_METHOD(void, present, (), (const, noexcept, override));
};
