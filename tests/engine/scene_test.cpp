#include "engine/scene.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_scene.h"

#include "gmock/gmock.h"
#include <gtest/gtest.h>

TEST(SceneManager, CallsOnAttachWhenAdded)
{
    ccsakura::scene_manager scene_mgr;

    auto mock_scene1 = std::make_unique<mock_scene>();
    EXPECT_CALL(*mock_scene1, on_attach).Times(1);
    EXPECT_CALL(*mock_scene1, on_detach).Times(0);

    scene_mgr.push_front(std::move(mock_scene1));
}

TEST(SceneManager, CallsOnDetachWhenRemoved)
{
    ccsakura::scene_manager scene_mgr;

    auto mock_scene1 = std::make_unique<mock_scene>();
    EXPECT_CALL(*mock_scene1, on_attach).Times(1);
    EXPECT_CALL(*mock_scene1, on_detach).Times(1);

    scene_mgr.push_front(std::move(mock_scene1));
    scene_mgr.pop_front();
}

TEST(SceneManager, RendersFromBottomToTop)
{
    ccsakura::scene_manager scene_mgr;

    mock_renderer mock_renderer;
    auto s1 = std::make_unique<mock_scene>();
    auto s2 = std::make_unique<mock_scene>();
    auto s3 = std::make_unique<mock_scene>();

    EXPECT_CALL(*s1, on_attach).Times(1);
    EXPECT_CALL(*s2, on_attach).Times(1);
    EXPECT_CALL(*s3, on_attach).Times(1);

    {
        using ::testing::InSequence;
        InSequence seq;
        EXPECT_CALL(*s3, on_render(testing::Ref(mock_renderer)));
        EXPECT_CALL(*s2, on_render(testing::Ref(mock_renderer)));
        EXPECT_CALL(*s1, on_render(testing::Ref(mock_renderer)));
    }

    scene_mgr.push_back(std::move(s1));
    scene_mgr.push_back(std::move(s2));
    scene_mgr.push_back(std::move(s3));

    scene_mgr.render(mock_renderer);
}

TEST(SceneManager, TicksFromTopToBottom)
{
    ccsakura::scene_manager scene_mgr;

    auto s1 = std::make_unique<mock_scene>();
    auto s2 = std::make_unique<mock_scene>();
    auto s3 = std::make_unique<mock_scene>();
    auto s4 = std::make_unique<mock_scene>();

    EXPECT_CALL(*s1, on_attach).Times(1);
    EXPECT_CALL(*s2, on_attach).Times(1);
    EXPECT_CALL(*s3, on_attach).Times(1);
    EXPECT_CALL(*s4, on_attach).Times(1);

    {
        using ::testing::InSequence;
        InSequence seq;
        EXPECT_CALL(*s1, on_tick).Times(1).WillOnce(testing::Return(true));
        EXPECT_CALL(*s2, on_tick).Times(1).WillOnce(testing::Return(true));
        EXPECT_CALL(*s3, on_tick).Times(1).WillOnce(testing::Return(false));
        EXPECT_CALL(*s4, on_tick).Times(0);
    }

    scene_mgr.push_back(std::move(s1));
    scene_mgr.push_back(std::move(s2));
    scene_mgr.push_back(std::move(s3));
    scene_mgr.push_back(std::move(s4));

    scene_mgr.tick(20);
}
