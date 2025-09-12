#pragma once

#include "sdl/sdl_video.h"

#include <gmock/gmock.h>

class mock_window : public sdl::iwindow
{
  public:
    MOCK_METHOD(SDL_Window *, get, (), (const, noexcept, override));
};
