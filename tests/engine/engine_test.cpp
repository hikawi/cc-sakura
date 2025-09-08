#include "engine/engine.h"
#include "mocks/mock_app.h"
#include "mocks/mock_renderer.h"
#include "sdl/sdl_render.h"

#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(Engine, ThrowsWithNullApp)
{
    using namespace ::testing;

    std::unique_ptr<mock_app> app_unique_ptr;
    ASSERT_THROW(std::unique_ptr<ccsakura::iengine> engine =
                     std::make_unique<ccsakura::engine>(std::move(app_unique_ptr)),
                 std::runtime_error);
}

TEST(Engine, HasToPresentOnce)
{
    using namespace ::testing;

    std::unique_ptr<mock_app> app_unique_ptr = std::make_unique<mock_app>();
    mock_app *app_ptr = app_unique_ptr.get();

    std::unique_ptr<sdl::irenderer> renderer = std::make_unique<mock_renderer>();
    EXPECT_CALL(*app_ptr, get_renderer()).WillRepeatedly(ReturnRef(*renderer));
    EXPECT_CALL(static_cast<mock_renderer &>(*renderer), present()).Times(1);

    ASSERT_NO_THROW({
        std::unique_ptr<ccsakura::iengine> engine = std::make_unique<ccsakura::engine>(std::move(app_unique_ptr));
        engine->render();
    });
}

TEST(Engine, CanCountFPS)
{
    using namespace ::testing;

    std::unique_ptr<mock_app> app_unique_ptr = std::make_unique<mock_app>();
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
