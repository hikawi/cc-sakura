#include "engine/component/sprite.h"
#include "engine/component/transform.h"
#include "engine/entity.h"

#include <gtest/gtest.h>

using namespace ccsakura;

TEST(EntityTest, CanAddComponent)
{
    entity e(1);
    e.add_component<components::transform>(vec2d(10, 20), 0.5);

    EXPECT_TRUE(e.has_component<components::transform>());

    const auto &t = e.get_component<components::transform>();
    EXPECT_DOUBLE_EQ(t->position.x, 10);
    EXPECT_DOUBLE_EQ(t->position.y, 20);
    EXPECT_DOUBLE_EQ(t->rotation, 0.5);
}

TEST(EntityTest, CanAddSpriteComponent)
{
    entity e(1);
    e.add_component<components::sprite>(nullptr, render_origin::bottom_center);

    EXPECT_TRUE(e.has_component<components::sprite>());
    const auto &s = e.get_component<components::sprite>();
    EXPECT_EQ(s->spr, nullptr);
    EXPECT_EQ(s->origin, render_origin::bottom_center);
}
