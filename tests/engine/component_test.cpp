#include "engine/component/sprite.h"
#include "mocks/mock_sprite.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace ccsakura;
using namespace ::testing;

namespace
{

TEST(SpriteComponent, TickAdvancesFrame)
{
    mock_sprite mock;
    sprite_frame frame0{.duration = 100};
    sprite_frame frame1{.duration = 100};

    EXPECT_CALL(mock, frame_count()).WillRepeatedly(Return(2));
    EXPECT_CALL(mock, frame(0)).WillRepeatedly(ReturnRef(frame0));
    EXPECT_CALL(mock, frame(1)).WillRepeatedly(ReturnRef(frame1));

    components::sprite sprite_comp(&mock);
    sprite_comp.tick(0.05); // 50ms
    EXPECT_EQ(sprite_comp.frame_index, 0);
    EXPECT_DOUBLE_EQ(sprite_comp.frame_elapsed, 0.05);

    sprite_comp.tick(0.06); // total 110ms
    EXPECT_EQ(sprite_comp.frame_index, 1);
    EXPECT_NEAR(sprite_comp.frame_elapsed, 0.01, 1e-9);
}

TEST(SpriteComponent, TickReverseAdvancesFrame)
{
    mock_sprite mock;
    sprite_frame frame0{.duration = 100};
    sprite_frame frame1{.duration = 100};

    EXPECT_CALL(mock, frame_count()).WillRepeatedly(Return(2));
    EXPECT_CALL(mock, frame(0)).WillRepeatedly(ReturnRef(frame0));
    EXPECT_CALL(mock, frame(1)).WillRepeatedly(ReturnRef(frame1));

    components::sprite sprite_comp(&mock);
    sprite_comp.reverse = true;
    sprite_comp.tick(0.11);
    EXPECT_EQ(sprite_comp.frame_index, 1); // 0 -> 1 in reverse wrap
    EXPECT_NEAR(sprite_comp.frame_elapsed, 0.01, 1e-9);

    sprite_comp.tick(0.1);
    EXPECT_EQ(sprite_comp.frame_index, 0); // 1 -> 0 in reverse
}

TEST(SpriteComponent, TickWithTag)
{
    mock_sprite mock;
    sprite_frame frame1{.duration = 100};
    sprite_frame_tag tag{.name = "walk", .from = 1, .to = 2};

    EXPECT_CALL(mock, frame(1)).WillRepeatedly(ReturnRef(frame1));
    EXPECT_CALL(mock, frame_tag("walk")).WillRepeatedly(ReturnRef(tag));

    components::sprite sprite_comp(&mock);
    sprite_comp.tag = "walk";
    sprite_comp.frame_index = 1;
    sprite_comp.tick(0.11);
    EXPECT_EQ(sprite_comp.frame_index, 2);
}

TEST(SpriteComponent, TickWithTagReverse)
{
    mock_sprite mock;
    sprite_frame frame1{.duration = 100};
    sprite_frame_tag tag{.name = "walk", .from = 1, .to = 2};

    EXPECT_CALL(mock, frame(1)).WillRepeatedly(ReturnRef(frame1));
    EXPECT_CALL(mock, frame_tag("walk")).WillRepeatedly(ReturnRef(tag));

    components::sprite sprite_comp(&mock);
    sprite_comp.tag = "walk";
    sprite_comp.frame_index = 1;
    sprite_comp.reverse = true;
    sprite_comp.tick(0.11);
    EXPECT_EQ(sprite_comp.frame_index, 2); // 1 -> 2 in reverse wrap
}

} // namespace
