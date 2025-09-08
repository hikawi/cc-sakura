#pragma once

#include "sdl/sdl_video.h"

#include <gmock/gmock.h>

class window_mock : public sdl::iwindow
{
  public:
    MOCK_METHOD(SDL_Window *, get, (), (const, noexcept, override));
};
