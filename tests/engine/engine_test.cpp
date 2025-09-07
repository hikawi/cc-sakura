#include "app.h"
#include "engine/engine.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_video.h"

#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

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
    MOCK_METHOD(std::unique_ptr<sdl::itexture>, create_texture,
                (sdl::pixel_format format, sdl::texture_access access, int w, int h), (const override));
    MOCK_METHOD(void, set_color, (const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a),
                (const override));
    MOCK_METHOD(void, set_color, (const float r, const float g, const float b, const float a), (const override));
    MOCK_METHOD(void, clear, (), (const override));
    MOCK_METHOD(void, present, (), (const override));
};

class app_mock : public ccsakura::iapp
{
  public:
    MOCK_METHOD(const sdl::irenderer &, get_renderer, (), (const override));
    MOCK_METHOD(const sdl::iwindow &, get_window, (), (const override));

    sdl::irenderer &get_mock_renderer() const
    {
        return *irenderer;
    }

  private:
    std::unique_ptr<sdl::iwindow> iwindow = std::make_unique<window_mock>();
    std::unique_ptr<sdl::irenderer> irenderer = std::make_unique<renderer_mock>();
};

// ==========================================

TEST(Engine, ThrowsWithNullApp)
{
    using namespace ::testing;

    std::unique_ptr<app_mock> app_unique_ptr;
    ASSERT_THROW(std::unique_ptr<ccsakura::iengine> engine =
                     std::make_unique<ccsakura::engine>(std::move(app_unique_ptr)),
                 std::runtime_error);
}

TEST(Engine, HasToPresentOnce)
{
    using namespace ::testing;

    std::unique_ptr<app_mock> app_unique_ptr = std::make_unique<app_mock>();
    app_mock *app_ptr = app_unique_ptr.get();

    sdl::irenderer &renderer = app_ptr->get_mock_renderer();
    EXPECT_CALL(*app_ptr, get_renderer()).WillRepeatedly(ReturnRef(renderer));
    EXPECT_CALL(static_cast<renderer_mock &>(renderer), present()).Times(1);

    ASSERT_NO_THROW({
        std::unique_ptr<ccsakura::iengine> engine = std::make_unique<ccsakura::engine>(std::move(app_unique_ptr));
        engine->render();
    });
}

TEST(Engine, CanCountFPS)
{
    using namespace ::testing;

    std::unique_ptr<app_mock> app_unique_ptr = std::make_unique<app_mock>();
    std::unique_ptr<ccsakura::iengine> engine = std::make_unique<ccsakura::engine>(std::move(app_unique_ptr));

    for (uint64_t i = 0; i < 1000; i += 3)
    {
        engine->iterate(i);
    }

    ASSERT_LT(engine->get_frame_data().accumulator, 1);
    ASSERT_EQ(engine->get_frame_data().cur_frames, 334);
    ASSERT_EQ(engine->get_frame_data().fps, 0); // not set yet

    engine->iterate(1001);
    ASSERT_EQ(engine->get_frame_data().fps, 335);
    ASSERT_EQ(engine->get_frame_data().cur_frames, 0);
}

} // namespace
