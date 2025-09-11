#pragma once

#include "sdl/sdl_ttf.h"

#include <gmock/gmock.h>

class mock_font : public sdl::ttf::font
{
  public:
    MOCK_METHOD(TTF_Font *, get, (), (const, noexcept, override));
    MOCK_METHOD(void, set_style, (const sdl::ttf::font_style), (const, noexcept, override));
    MOCK_METHOD(void, set_size, (const float), (const, noexcept, override));
    MOCK_METHOD(void, set_hint, (const sdl::ttf::font_hint), (const, noexcept, override));
    MOCK_METHOD(std::unique_ptr<sdl::isurface>, render_text_blended, (const std::string, const sdl::color),
                (const, noexcept, override));
};
