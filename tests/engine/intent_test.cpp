#include "engine/intent.h"
#include "engine/scene_manager.h"
#include "engine/signal.h"
#include "sdl/sdl_events.h"

#include <gtest/gtest.h>

using namespace ccsakura;

TEST(IntentBinder, CallsCommitOnBind)
{
    bool committed = false;
    intent_binder(
        [&committed](std::vector<std::pair<sdl::keycode, intent>> bindings)
        {
            committed = true;
            EXPECT_EQ(bindings.size(), 1);
            EXPECT_EQ(bindings[0].first, sdl::keycode::a);
            EXPECT_EQ(bindings[0].second, intent::move_left);
        })
        .map(sdl::keycode::a, intent::move_left)
        .bind();

    EXPECT_TRUE(committed);
}

TEST(IntentBinder, CollectsMultipleBindings)
{
    std::vector<std::pair<sdl::keycode, intent>> captured;
    intent_binder([&captured](std::vector<std::pair<sdl::keycode, intent>> bindings)
                  { captured = std::move(bindings); })
        .map(sdl::keycode::a, intent::move_left)
        .map(sdl::keycode::d, intent::move_right)
        .map(sdl::keycode::w, intent::move_up)
        .map(sdl::keycode::s, intent::move_down)
        .bind();

    ASSERT_EQ(captured.size(), 4);
    EXPECT_EQ(captured[0], std::make_pair(sdl::keycode::a, intent::move_left));
    EXPECT_EQ(captured[1], std::make_pair(sdl::keycode::d, intent::move_right));
    EXPECT_EQ(captured[2], std::make_pair(sdl::keycode::w, intent::move_up));
    EXPECT_EQ(captured[3], std::make_pair(sdl::keycode::s, intent::move_down));
}

TEST(IntentBinder, CommitNotCalledWithoutBind)
{
    bool committed = false;
    intent_binder binder([&committed](auto) { committed = true; });
    binder.map(sdl::keycode::a, intent::move_left);
    // bind() not called
    EXPECT_FALSE(committed);
}

namespace
{

// Thin scene subclass that exposes protected intent API for testing.
class intent_test_scene : public iscene
{
  public:
    scene_type type() const noexcept override
    {
        return scene_type::dbg_none;
    }

    void setup(scene_context &ctx)
    {
        bind_intents(ctx).map(sdl::keycode::a, intent::move_left).map(sdl::keycode::d, intent::move_right).bind();
    }

    void teardown(scene_context &ctx)
    {
        unbind_intents(ctx);
    }

    bool triggered(intent i) const
    {
        return is_intent_triggered(i);
    }
};

static signals::key make_key_signal(sdl::keycode key, bool down)
{
    sdl::events::key data{};
    data.key = key;
    data.down = down;
    return signals::key(0, data);
}

} // namespace

TEST(SceneBindIntents, KeyDownTriggersIntent)
{
    scene_manager mgr;
    intent_test_scene scene;
    scene.setup(mgr);

    auto sig = make_key_signal(sdl::keycode::a, true);
    mgr.propagate_signals(sig);

    EXPECT_TRUE(scene.triggered(intent::move_left));
    EXPECT_FALSE(scene.triggered(intent::move_right));
}

TEST(SceneBindIntents, KeyUpClearsIntent)
{
    scene_manager mgr;
    intent_test_scene scene;
    scene.setup(mgr);

    auto down = make_key_signal(sdl::keycode::a, true);
    mgr.propagate_signals(down);
    EXPECT_TRUE(scene.triggered(intent::move_left));

    auto up = make_key_signal(sdl::keycode::a, false);
    mgr.propagate_signals(up);
    EXPECT_FALSE(scene.triggered(intent::move_left));
}

TEST(SceneBindIntents, UnmappedKeyDoesNotTrigger)
{
    scene_manager mgr;
    intent_test_scene scene;
    scene.setup(mgr);

    auto sig = make_key_signal(sdl::keycode::space, true);
    mgr.propagate_signals(sig);

    EXPECT_FALSE(scene.triggered(intent::move_left));
    EXPECT_FALSE(scene.triggered(intent::move_right));
    EXPECT_FALSE(scene.triggered(intent::move_up));
    EXPECT_FALSE(scene.triggered(intent::move_down));
}

TEST(SceneBindIntents, UnbindStopsReceivingSignals)
{
    scene_manager mgr;
    intent_test_scene scene;
    scene.setup(mgr);

    auto down = make_key_signal(sdl::keycode::a, true);
    mgr.propagate_signals(down);
    EXPECT_TRUE(scene.triggered(intent::move_left));

    scene.teardown(mgr);

    // After unbind, state is cleared and further signals have no effect.
    EXPECT_FALSE(scene.triggered(intent::move_left));
    mgr.propagate_signals(down);
    EXPECT_FALSE(scene.triggered(intent::move_left));
}
