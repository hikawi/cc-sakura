#pragma once

#include "app.h"

#include "gmock/gmock.h"

class mock_app : public ccsakura::iapp
{
  public:
    MOCK_METHOD(const sdl::irenderer &, get_renderer, (), (const, noexcept, override));
    MOCK_METHOD(const sdl::iwindow &, get_window, (), (const, noexcept, override));
};
