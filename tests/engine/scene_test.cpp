#include "engine/scene.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_scene.h"
#include "mocks/mock_texture.h"
#include "sdl/sdl_render.h"

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

    EXPECT_CALL(mock_renderer,
                create_texture(testing::An<sdl::pixel_format>(), testing::An<sdl::texture_access>(), 480, 270))
        .Times(3)
        .WillRepeatedly(testing::InvokeWithoutArgs([]() { return std::make_unique<mock_texture>(); }));

    // Once for the whole window
    // Three times, once for each scene
    EXPECT_CALL(mock_renderer, clear()).Times(4);

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

TEST(SceneTransition, CompletesAndCallsDetach)
{
    ccsakura::scene_manager scene_mgr;

    auto from = std::make_unique<mock_scene>();
    auto to = std::make_unique<mock_scene>();

    EXPECT_CALL(*from, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*from, on_pause(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*from, on_detach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*to, on_attach(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*to, on_start(testing::Ref(scene_mgr))).Times(1);
    EXPECT_CALL(*to, on_detach).Times(0);
    ON_CALL(*from, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_sprite));
    ON_CALL(*to, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_physics));
    EXPECT_CALL(*to, on_tick(testing::Ref(scene_mgr), testing::_)).WillRepeatedly(testing::Return(true));

    scene_mgr.push_back(std::move(from));
    scene_mgr.process_requests();

    scene_mgr.start_transition(std::move(to), ccsakura::scene_type::dbg_sprite, ccsakura::scene_transition::fade(0.5));
    scene_mgr.process_requests();

    // Tick past the duration — from_scene detached, to_scene started
    scene_mgr.tick(0.6);
}

TEST(SceneTransition, PauseCalledBeforeTicksFreezeAndStartAfterDetach)
{
    ccsakura::scene_manager scene_mgr;

    auto from = std::make_unique<mock_scene>();
    auto to = std::make_unique<mock_scene>();
    ON_CALL(*from, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_sprite));
    ON_CALL(*to, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_physics));

    std::vector<std::string> events;

    EXPECT_CALL(*from, on_attach).WillOnce([&](auto &) { events.push_back("from:attach"); });
    EXPECT_CALL(*from, on_pause).WillOnce([&](auto &) { events.push_back("from:pause"); });
    EXPECT_CALL(*from, on_detach).WillOnce([&](auto &) { events.push_back("from:detach"); });
    EXPECT_CALL(*to, on_attach).WillOnce([&](auto &) { events.push_back("to:attach"); });
    EXPECT_CALL(*to, on_start).WillOnce([&](auto &) { events.push_back("to:start"); });
    EXPECT_CALL(*to, on_tick).WillRepeatedly(testing::Return(true));

    scene_mgr.push_back(std::move(from));
    scene_mgr.process_requests();

    scene_mgr.start_transition(std::move(to), ccsakura::scene_type::dbg_sprite, ccsakura::scene_transition::fade(0.2));
    scene_mgr.process_requests();
    scene_mgr.tick(0.3);

    ASSERT_EQ(events.size(), 5u);
    EXPECT_EQ(events[0], "from:attach");
    EXPECT_EQ(events[1], "from:pause");
    EXPECT_EQ(events[2], "to:attach");
    EXPECT_EQ(events[3], "from:detach");
    EXPECT_EQ(events[4], "to:start");
}

TEST(SceneTransition, FadeAppliesAlphaMod)
{
    ccsakura::scene_manager scene_mgr;
    mock_renderer renderer;

    auto from = std::make_unique<mock_scene>();
    auto to = std::make_unique<mock_scene>();
    ON_CALL(*from, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_sprite));
    ON_CALL(*to, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_physics));
    EXPECT_CALL(*from, on_attach).Times(1);
    EXPECT_CALL(*to, on_attach).Times(1);

    // Textures created lazily in render(): from_scene first (rbegin order), then to_scene
    auto *from_tex = new mock_texture();
    auto *to_tex = new mock_texture();
    EXPECT_CALL(renderer,
                create_texture(testing::An<sdl::pixel_format>(), testing::An<sdl::texture_access>(), 480, 270))
        .WillOnce(testing::Return(std::unique_ptr<sdl::itexture>(from_tex)))
        .WillOnce(testing::Return(std::unique_ptr<sdl::itexture>(to_tex)));

    // Renderer boilerplate
    EXPECT_CALL(renderer, set_render_target).Times(testing::AnyNumber());
    EXPECT_CALL(renderer, set_color(testing::An<uint8_t>(), testing::An<uint8_t>(), testing::An<uint8_t>(),
                                    testing::An<uint8_t>()))
        .Times(testing::AnyNumber());
    EXPECT_CALL(renderer,
                set_color(testing::An<float>(), testing::An<float>(), testing::An<float>(), testing::An<float>()))
        .Times(testing::AnyNumber());
    EXPECT_CALL(renderer, clear).Times(testing::AnyNumber());
    EXPECT_CALL(renderer, render_texture).Times(testing::AnyNumber());

    // Texture setup calls (blend/scale mode set during lazy creation)
    EXPECT_CALL(*from_tex, set_blend_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*from_tex, set_scale_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*to_tex, set_blend_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*to_tex, set_scale_mode).Times(testing::AnyNumber());

    // At t=0.5: from gets alpha 127, to gets alpha 127; both get 255 reset
    EXPECT_CALL(*from_tex, set_alpha_mod(uint8_t{127})).Times(1);
    EXPECT_CALL(*from_tex, set_alpha_mod(uint8_t{255})).Times(1);
    EXPECT_CALL(*to_tex, set_alpha_mod(uint8_t{127})).Times(1);
    EXPECT_CALL(*to_tex, set_alpha_mod(uint8_t{255})).Times(1);

    scene_mgr.push_back(std::move(from));
    scene_mgr.process_requests();

    scene_mgr.start_transition(std::move(to), ccsakura::scene_type::dbg_sprite, ccsakura::scene_transition::fade(1.0));
    scene_mgr.process_requests();

    scene_mgr.tick(0.5);
    scene_mgr.render(renderer);
}

TEST(SceneTransition, HudUnaffectedByTransition)
{
    ccsakura::scene_manager scene_mgr;
    mock_renderer renderer;

    auto hud = std::make_unique<mock_scene>();
    auto from = std::make_unique<mock_scene>();
    auto to = std::make_unique<mock_scene>();
    ON_CALL(*hud, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_fps));
    ON_CALL(*from, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_sprite));
    ON_CALL(*to, type()).WillByDefault(testing::Return(ccsakura::scene_type::dbg_physics));
    EXPECT_CALL(*hud, on_attach).Times(1);
    EXPECT_CALL(*from, on_attach).Times(1);
    EXPECT_CALL(*to, on_attach).Times(1);

    // Stack after setup: [hud(front), to, from(back)] — 3 textures created in rbegin order
    auto *from_tex = new mock_texture();
    auto *to_tex = new mock_texture();
    auto *hud_tex = new mock_texture();

    // Lmao. What the fuck did Claude do here.
    // Gmock said it's not needed to do this, but who knows.
    EXPECT_CALL(renderer,
                create_texture(testing::An<sdl::pixel_format>(), testing::An<sdl::texture_access>(), 480, 270))
        .WillOnce(testing::Return(std::unique_ptr<sdl::itexture>(from_tex)))
        .WillOnce(testing::Return(std::unique_ptr<sdl::itexture>(to_tex)))
        .WillOnce(testing::Return(std::unique_ptr<sdl::itexture>(hud_tex)));

    EXPECT_CALL(renderer, set_render_target).Times(testing::AnyNumber());
    EXPECT_CALL(renderer, set_color(testing::An<uint8_t>(), testing::An<uint8_t>(), testing::An<uint8_t>(),
                                    testing::An<uint8_t>()))
        .Times(testing::AnyNumber());
    EXPECT_CALL(renderer,
                set_color(testing::An<float>(), testing::An<float>(), testing::An<float>(), testing::An<float>()))
        .Times(testing::AnyNumber());
    EXPECT_CALL(renderer, clear).Times(testing::AnyNumber());
    EXPECT_CALL(renderer, render_texture).Times(testing::AnyNumber());
    EXPECT_CALL(*from_tex, set_blend_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*from_tex, set_scale_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*to_tex, set_blend_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*to_tex, set_scale_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*hud_tex, set_blend_mode).Times(testing::AnyNumber());
    EXPECT_CALL(*hud_tex, set_scale_mode).Times(testing::AnyNumber());

    // HUD must only receive alpha=255 (no transition effect)
    EXPECT_CALL(*hud_tex, set_alpha_mod(uint8_t{255})).Times(testing::AtLeast(1));
    EXPECT_CALL(*hud_tex, set_alpha_mod(testing::Ne(uint8_t{255}))).Times(0);

    scene_mgr.push_front(std::move(hud)); // front = highest layer
    scene_mgr.push_back(std::move(from));
    scene_mgr.process_requests();

    scene_mgr.start_transition(std::move(to), ccsakura::scene_type::dbg_sprite,
                               ccsakura::scene_transition::slide_left(1.0));
    scene_mgr.process_requests();

    scene_mgr.tick(0.5);
    scene_mgr.render(renderer);
}

TEST(SceneTransition, NoFromSceneDoesNothing)
{
    ccsakura::scene_manager scene_mgr;

    auto to = std::make_unique<mock_scene>();
    EXPECT_CALL(*to, on_attach).Times(0); // must not be attached if from not found
    EXPECT_CALL(*to, on_detach).Times(0);

    // start_transition targeting a type not present in the stack
    scene_mgr.start_transition(std::move(to), ccsakura::scene_type::dbg_sprite, ccsakura::scene_transition::fade(1.0));
    scene_mgr.process_requests();
    // No crash, no transition started
}
