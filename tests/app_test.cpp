#include "app.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_video.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

class window_mock : public sdl::iwindow
{
  public:
    SDL_Window *get() const override
    {
        return reinterpret_cast<SDL_Window *>(0xAAAABBBBAAAABBBB);
    }
};

class renderer_mock : public sdl::irenderer
{
  public:
    MOCK_METHOD(void, set_color, (const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a),
                (const override));
    MOCK_METHOD(void, set_color, (const float r, const float g, const float b, const float a), (const override));
    MOCK_METHOD(void, clear, (), (const override));
    MOCK_METHOD(void, present, (), (const override));
};

// ====================================

TEST(App, CrashWithNullWindow)
{
    std::unique_ptr<sdl::iwindow> window;
    std::unique_ptr<sdl::irenderer> renderer = std::make_unique<renderer_mock>();

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
    std::unique_ptr<sdl::irenderer> renderer = std::make_unique<renderer_mock>();

    ASSERT_NO_THROW(std::make_unique<ccsakura::app>(std::move(window), std::move(renderer)));
}
