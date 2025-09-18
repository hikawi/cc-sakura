#include "engine/colllision.h"
#include "engine/vec2d.h"
#include "extensions/gtest_exts.h"

#include <cmath>
#include <gtest/gtest.h>
#include <numbers>

TEST(AABBCollider, NotCollidingWithAABBCollider)
{
    ccsakura::aabb_collider a(0, 0, 4, 4);
    ccsakura::aabb_collider b(0, 5, 4, 4);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, CollidingWithAABBCollider)
{
    // Collides a little from the left.
    ccsakura::aabb_collider a(0, 0, 4, 4);
    ccsakura::aabb_collider b(-2, 0, 1, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(-1, 0));
}

TEST(AABBCollider, NotCollidingWithOBBCollider)
{
    ccsakura::aabb_collider a(0, 0, 2, 2);
    ccsakura::obb_collider b(3, 3, 2, 2, std::numbers::pi / 4);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, CollidingWithOBBCollider)
{
    ccsakura::aabb_collider a(0, 0, 4, 2);
    ccsakura::obb_collider b(-2, 0, 1, 2, std::numbers::pi / 4);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(-1, 0));
}

TEST(AABBCollider, NotCollidingWithCircleCollider)
{
    ccsakura::aabb_collider a(0, 0, 4, 4);
    ccsakura::circle_collider b({3, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, CollidingWithCircleCollider)
{
    ccsakura::aabb_collider a(0, 0, 4, 4);
    ccsakura::circle_collider b({0, 2.3}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.7);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, 1));
}

TEST(AABBCollider, NotCollidingWithCapsuleCollider)
{
    ccsakura::aabb_collider a(0, 0, 4, 4);
    ccsakura::capsule_collider b({0, 3}, {5, 5}, 0.9);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, CollidingWithCapsuleColliderInside)
{
    ccsakura::aabb_collider a(0, 0, 4, 4);
    ccsakura::capsule_collider b({0, 2}, {0, 4}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, 1));
}

TEST(AABBCollider, CollidingWithCapsuleColliderOutside)
{
    ccsakura::aabb_collider a(0, 0, 4, 4);
    ccsakura::capsule_collider b({0, -2.5}, {0, -5}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, -1));
}

TEST(OBBCollider, NotCollidingWithOBBCollider)
{
    ccsakura::obb_collider a(0, 0, 4, 4, 0.3);
    ccsakura::obb_collider b(5, 5, 4, 4, 0.6);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, CollidingWithOBBCollider)
{
    // Collides a bit on the right.
    ccsakura::obb_collider a(0, 0, 4, 4, std::numbers::pi / 4);
    ccsakura::obb_collider b(4, 0, 4, 4, std::numbers::pi / 4);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 4 - 2 * std::sqrt(2));

    // There can be 2 normals (pi/4, -pi/4) or (pi/4, pi/4)
    ccsakura::vec2d angle1(std::numbers::pi / 4, -std::numbers::pi / 4);
    angle1 = angle1.normalized();
    ccsakura::vec2d angle2(std::numbers::pi / 4, std::numbers::pi / 4);
    angle2 = angle2.normalized();
    ASSERT_TRUE(ab.normal == angle1 || ab.normal == angle2);
}

TEST(OBBCollider, NotCollidingWithCircleCollider)
{
    ccsakura::obb_collider a(0, 0, 4, 4, std::numbers::pi / 4);
    ccsakura::circle_collider b({4, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, CollidingWithCircleCollider)
{
    ccsakura::obb_collider a(0, 0, 4, 4, std::numbers::pi / 4);
    ccsakura::circle_collider b({3, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_NEAR(ab.depth, 2 * std::sqrt(2) - 2, 0.001);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(1, 0));
}

TEST(OBBCollider, NotCollidingWithCapsuleCollider)
{
    ccsakura::obb_collider a(0, 0, 4, 4, std::numbers::pi / 4);
    ccsakura::capsule_collider b({4, 4}, {6, 6}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, CollidingWithCapsuleCollider)
{
    ccsakura::obb_collider a(0, 0, 4, 4, std::numbers::pi / 4);
    ccsakura::capsule_collider b({0, 3}, {0, 5}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, 1));
}

TEST(CircleCollider, NotCollidingWithCircleCollider)
{
    ccsakura::circle_collider a({0, 0}, 2);
    ccsakura::circle_collider b({-5, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, CollidingWithCircleCollider)
{
    ccsakura::circle_collider a({0, 0}, 2);
    ccsakura::circle_collider b({-2.5, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(-1, 0));
}

TEST(CircleCollider, NotCollidingWithCapsuleCollider)
{
    ccsakura::circle_collider a({0, 0}, 2);
    ccsakura::capsule_collider b({0, -4}, {0, -8}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, CollidingWithCapsuleCollider)
{
    ccsakura::circle_collider a({0, 0}, 2);
    ccsakura::capsule_collider b({0, -2}, {0, -4}, 0.5);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(0, -1));
}

TEST(CapsuleCollider, NotCollidingWithCapsuleCollider)
{
    ccsakura::capsule_collider a({0, 0}, {-2, 0}, 1);
    ccsakura::capsule_collider b({-5, 0}, {-7, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CapsuleCollider, CollidingWithCapsuleCollider)
{
    ccsakura::capsule_collider a({0, 0}, {-2, 0}, 1);
    ccsakura::capsule_collider b({-2.5, 0}, {-5, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 1.5);
    ASSERT_EQ(ab.normal, ccsakura::vec2d(-1, 0));
}
