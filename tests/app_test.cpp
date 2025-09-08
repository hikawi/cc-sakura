#include "app.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_window.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_video.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

TEST(App, CrashWithNullWindow)
{
    std::unique_ptr<sdl::iwindow> window;
    std::unique_ptr<sdl::irenderer> renderer = std::make_unique<mock_renderer>();

    ASSERT_THROW(std::make_unique<ccsakura::app>(std::move(window), std::move(renderer)), std::runtime_error);
}

TEST(App, CrashWithNullRenderer)
{
    std::unique_ptr<sdl::iwindow> window = std::make_unique<window_mock>();
    std::unique_ptr<sdl::irenderer> renderer;

    ASSERT_THROW(std::make_unique<ccsakura::app>(std::move(window), std::move(renderer)), std::runtime_error);
}

TEST(App, DoesNotCrashWithBothValid)
{
    std::unique_ptr<sdl::iwindow> window = std::make_unique<window_mock>();
    std::unique_ptr<sdl::irenderer> renderer = std::make_unique<mock_renderer>();

    ASSERT_NO_THROW(std::make_unique<ccsakura::app>(std::move(window), std::move(renderer)));
}
