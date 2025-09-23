#include "engine/engine.h"
#include "mocks/mock_app.h"
#include "mocks/mock_font_cache.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_sprite_cache.h"
#include "mocks/mock_window.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

static ccsakura::engine_deps setup_deps()
{
    ccsakura::engine_deps deps;

    deps.m_window = std::make_unique<mock_window>();
    deps.m_renderer = std::make_unique<mock_renderer>();
    deps.m_app = std::make_unique<mock_app>();
    deps.m_font_cache = std::make_unique<mock_font_cache>();
    deps.m_sprite_cache = std::make_unique<mock_sprite_cache>();

    return deps;
}

TEST(Engine, ThrowsWithNullDependencies)
{
    using namespace ::testing;

    ccsakura::engine_deps deps;
    ASSERT_THROW(std::unique_ptr<ccsakura::iengine> engine = std::make_unique<ccsakura::engine>(std::move(deps)),
                 std::runtime_error);
}

TEST(Engine, CanCountFPS)
{
    using namespace ::testing;

    ccsakura::engine_deps deps = setup_deps();
    std::unique_ptr<ccsakura::iengine> engine = std::make_unique<ccsakura::engine>(std::move(deps));

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
