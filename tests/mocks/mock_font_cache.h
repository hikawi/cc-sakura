#pragma once

#include "engine/text.h"

#include <gmock/gmock.h>

class mock_font_cache : public ccsakura::ifont_cache
{
  public:
    MOCK_METHOD(sdl::ttf::ifont &, BracketOp, (const ccsakura::font), ());
    MOCK_METHOD(void, clear, (), (override));

    sdl::ttf::ifont &operator[](const ccsakura::font font) override
    {
        return BracketOp(font);
    }
};
