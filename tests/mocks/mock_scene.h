#pragma once

#include "engine/scene.h"
#include "sdl/sdl_render.h"

#include <gmock/gmock.h>

class mock_scene : public ccsakura::iscene
{
  public:
    MOCK_METHOD(ccsakura::scene_type, type, (), (const, noexcept, override));
    MOCK_METHOD(void, on_attach, (ccsakura::scene_context &), (override));
    MOCK_METHOD(void, on_detach, (ccsakura::scene_context &), (override));
    MOCK_METHOD(bool, on_tick, (ccsakura::scene_context &, const double), (noexcept, override));
    MOCK_METHOD(bool, on_physical_tick, (ccsakura::scene_context &), (noexcept, override));
    MOCK_METHOD(void, on_render, (const sdl::irenderer &), (const, noexcept, override));
};
