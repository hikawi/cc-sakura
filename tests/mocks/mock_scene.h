#pragma once

#include "engine/scene.h"
#include "sdl/sdl_render.h"

#include <gmock/gmock.h>

class mock_scene : public ccsakura::iscene
{
  public:
    MOCK_METHOD(ccsakura::scene_type, type, (), (const, noexcept, override));
    MOCK_METHOD(void, on_attach, (), (override));
    MOCK_METHOD(void, on_detach, (), (override));
    MOCK_METHOD(bool, on_tick, (const double), (noexcept, override));
    MOCK_METHOD(bool, on_physical_tick, (), (noexcept, override));
    MOCK_METHOD(void, on_render, (const sdl::irenderer &), (const, noexcept, override));
};
