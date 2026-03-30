#include "engine/collision.h"
#include "engine/vec2d.h"
#include "extensions/gtest_exts.h"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(AABBCollider, FailsWithNegativeExtents)
{
    ASSERT_THROW(ccsakura::aabb_collider({0, 0}, {-1, -1}), std::invalid_argument);
}

TEST(OBBCollider, FailsWithNegativeExtents)
{
    ASSERT_THROW(ccsakura::obb_collider({0, 0}, {-1, -1}, 0), std::invalid_argument);
}

TEST(CircleCollider, FailsWithNegativeRadius)
{
    ASSERT_THROW(ccsakura::circle_collider({0, 0}, -1), std::invalid_argument);
}

TEST(CapsuleCollider, FailsWithNegativeRadius)
{
    ASSERT_THROW(ccsakura::capsule_collider({0, 0}, {5, 0}, -2), std::invalid_argument);
}

TEST(AABBCollider, VsAABBNoCollision)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::aabb_collider b({0, -4}, {1, 1});

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsAABBCollideOnYAxis)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::aabb_collider b({0, -1.5}, {1, 1});

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, -1));
}

TEST(AABBCollider, VsAABBCollideOnXAxis)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::aabb_collider b({-1.3, 0}, {1, 1});

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.7);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(-1, 0));
}

TEST(AABBCollider, VsAABBTouching)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::aabb_collider b({2, 0}, {1, 1});

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0);
}

TEST(AABBCollider, VsCircleNoCollision)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::circle_collider b({5, 5}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsCircleCollideNormally)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::circle_collider b({0, 1.5}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, 1));
}

TEST(AABBCollider, VsCircleOverlap)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::circle_collider b({0.7, 0}, 0.3);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.6);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(1, 0));
}

TEST(AABBCollider, VsCapsuleNoCollision)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::capsule_collider b({-4, 0}, {-8, 1}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsCapsuleCollideOutside)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::capsule_collider b({1.5, 0}, {1.5, 4}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(1, 0));
}

TEST(AABBCollider, VsCapsuleOverlap)
{
    ccsakura::aabb_collider a({0, 0}, {1, 1});
    ccsakura::capsule_collider b({-1, 0}, {-4, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 1);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(-1, 0));
}

TEST(OBBCollider, VsOBBNoCollision)
{
    ccsakura::obb_collider a({0, 0}, {1, 1}, std::numbers::pi / 4);
    ccsakura::obb_collider b({0, -4}, {1, 1}, std::numbers::pi / 4);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsOBBCollide)
{
    ccsakura::obb_collider a({0, 0}, {1, 1}, std::numbers::pi / 4);
    ccsakura::obb_collider b({1.5, 0}, {1, 1}, std::numbers::pi / 4);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);

    b.shift(ab.normal * ab.depth);
    ab = a.collides(b);
    ASSERT_DOUBLE_EQ(ab.depth, 0);
}

TEST(OBBCollider, VsCircleNoCollision)
{
    ccsakura::obb_collider a({0, 0}, {1, 1}, std::numbers::pi / 6);
    ccsakura::circle_collider b({3, 3}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsCircleCollide)
{
    ccsakura::obb_collider a({0, 0}, {1, 1}, std::numbers::pi / 6);
    ccsakura::circle_collider b({0, 1}, 0.4);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);

    b.shift(ab.normal * (ab.depth + 0.0001));
    ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsCapsuleNoCollision)
{
    ccsakura::obb_collider a({0, 0}, {1, 1}, std::numbers::pi / 8);
    ccsakura::capsule_collider b({4, 4}, {5, 5}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsCapsuleCollide)
{
    ccsakura::obb_collider a({0, 0}, {1, 1}, std::numbers::pi / 8);
    ccsakura::capsule_collider b({0.4, 0.4}, {2, 2}, 0.2);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);

    b.shift(ab.normal * (ab.depth + 0.001));
    ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, VsCircleNoCollision)
{
    ccsakura::circle_collider a({0, 0}, 1);
    ccsakura::circle_collider b({-4, 4}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, VsCircleCollide)
{
    ccsakura::circle_collider a({0, 0}, 1);
    ccsakura::circle_collider b({0, -1}, 0.2);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.2);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, -1));
}

TEST(CircleCollider, VsCapsuleNoCollision)
{
    ccsakura::circle_collider a({0, 0}, 1);
    ccsakura::capsule_collider b({-2, 0}, {-5, 0}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, VsCapsuleCollide)
{
    ccsakura::circle_collider a({0, 0}, 1);
    ccsakura::capsule_collider b({-1, 0}, {-5, 0}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(-1, 0));
}

TEST(CapsuleCollider, VsCapsuleNoCollision)
{
    ccsakura::capsule_collider a({0, 0}, {2, 0}, 1);
    ccsakura::capsule_collider b({5, 0}, {8, 0}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CapsuleCollider, VsCapsuleCollide)
{
    ccsakura::capsule_collider a({0, 0}, {2, 0}, 1);
    ccsakura::capsule_collider b({3, 0}, {8, 0}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(1, 0));
}
