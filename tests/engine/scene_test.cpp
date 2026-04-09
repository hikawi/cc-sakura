#include "engine/scene.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_scene.h"

#include "gmock/gmock.h"
#include <gtest/gtest.h>

TEST(SceneManager, CallsOnAttachWhenAdded)
{
    ccsakura::scene_manager scene_mgr;

    auto mock_scene1 = std::make_unique<mock_scene>();
    EXPECT_CALL(*mock_scene1, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*mock_scene1, on_detach).Times(0);

    scene_mgr.push_front(std::move(mock_scene1));
    scene_mgr.process_requests();
}

TEST(SceneManager, CallsOnDetachWhenRemoved)
{
    ccsakura::scene_manager scene_mgr;

    auto mock_scene1 = std::make_unique<mock_scene>();
    EXPECT_CALL(*mock_scene1, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*mock_scene1, on_detach(testing::Ref(scene_mgr))).Times(1);

    scene_mgr.push_front(std::move(mock_scene1));
    scene_mgr.pop_front();
    scene_mgr.process_requests();
}

TEST(SceneManager, RenderClearsWithBackgroundColor)
{
    ccsakura::scene_manager scene_mgr;

    mock_renderer mock_renderer;
    auto s1 = std::make_unique<mock_scene>();
    auto s2 = std::make_unique<mock_scene>();
    auto s3 = std::make_unique<mock_scene>();

    EXPECT_CALL(*s1, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*s2, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*s3, on_attach(testing::Ref(scene_mgr))).Times(1);

    EXPECT_CALL(mock_renderer, set_color(testing::An<float>(), testing::An<float>(), testing::An<float>(),
                                        testing::An<float>()))
        .Times(1);
    EXPECT_CALL(mock_renderer, clear()).Times(1);

    scene_mgr.push_back(std::move(s1));
    scene_mgr.push_back(std::move(s2));
    scene_mgr.push_back(std::move(s3));
    scene_mgr.process_requests();

    scene_mgr.render(mock_renderer);
}

TEST(SceneManager, TicksFromTopToBottom)
{
    ccsakura::scene_manager scene_mgr;

    auto s1 = std::make_unique<mock_scene>();
    auto s2 = std::make_unique<mock_scene>();
    auto s3 = std::make_unique<mock_scene>();
    auto s4 = std::make_unique<mock_scene>();

    EXPECT_CALL(*s1, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*s2, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*s3, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*s4, on_attach(testing::Ref(scene_mgr))).Times(1);

    {
        using ::testing::InSequence;
        InSequence seq;
        EXPECT_CALL(*s1, on_tick(testing::Ref(scene_mgr), testing::_)).Times(1).WillOnce(testing::Return(true));
        EXPECT_CALL(*s2, on_tick(testing::Ref(scene_mgr), testing::_)).Times(1).WillOnce(testing::Return(true));
        EXPECT_CALL(*s3, on_tick(testing::Ref(scene_mgr), testing::_)).Times(1).WillOnce(testing::Return(false));
        EXPECT_CALL(*s4, on_tick).Times(0);
    }

    scene_mgr.push_back(std::move(s1));
    scene_mgr.push_back(std::move(s2));
    scene_mgr.push_back(std::move(s3));
    scene_mgr.push_back(std::move(s4));
    scene_mgr.process_requests();

    scene_mgr.tick(20);
}

TEST(SceneManager, PropagatesSignalsToSubscribers)
{
    ccsakura::scene_manager scene_mgr;
    bool called = false;

    scene_mgr.subscribe<ccsakura::signals::quit>(ccsakura::listener_priority::normal,
                                                 [&called](const ccsakura::signals::quit &) { called = true; });

    ccsakura::signals::quit quit_sig(123);
    scene_mgr.propagate_signals(quit_sig);

    EXPECT_TRUE(called);
}

TEST(SceneManager, SubscribesMemberFunction)
{
    class signal_receiver
    {
      public:
        void handle_quit(ccsakura::signals::quit &)
        {
            called = true;
        }
        bool called = false;
    };

    ccsakura::scene_manager scene_mgr;
    signal_receiver receiver;

    scene_mgr.subscribe(ccsakura::listener_priority::normal, &signal_receiver::handle_quit, &receiver);

    ccsakura::signals::quit quit_sig(123);
    scene_mgr.propagate_signals(quit_sig);

    EXPECT_TRUE(receiver.called);
}

TEST(SceneManager, UnsubscribesListener)
{
    ccsakura::scene_manager scene_mgr;
    int call_count = 0;

    auto id = scene_mgr.subscribe<ccsakura::signals::quit>(
        ccsakura::listener_priority::normal, [&call_count](const ccsakura::signals::quit &) { call_count++; });

    ccsakura::signals::quit quit_sig(123);
    scene_mgr.propagate_signals(quit_sig);
    EXPECT_EQ(call_count, 1);

    scene_mgr.unsubscribe(id);
    scene_mgr.propagate_signals(quit_sig);
    EXPECT_EQ(call_count, 1);
}

TEST(SceneManager, RespectsListenerPriority)
{
    ccsakura::scene_manager scene_mgr;
    std::vector<int> execution_order;

    scene_mgr.subscribe<ccsakura::signals::quit>(ccsakura::listener_priority::high,
                                                 [&execution_order](const auto &) { execution_order.push_back(2); });
    scene_mgr.subscribe<ccsakura::signals::quit>(ccsakura::listener_priority::normal,
                                                 [&execution_order](const auto &) { execution_order.push_back(1); });
    scene_mgr.subscribe<ccsakura::signals::quit>(ccsakura::listener_priority::lowest,
                                                 [&execution_order](const auto &) { execution_order.push_back(0); });

    ccsakura::signals::quit quit_sig(123);
    scene_mgr.propagate_signals(quit_sig);

    ASSERT_EQ(execution_order.size(), 3);
    EXPECT_EQ(execution_order[0], 0);
    EXPECT_EQ(execution_order[1], 1);
    EXPECT_EQ(execution_order[2], 2);
}
