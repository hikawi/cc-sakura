#include "engine/collision.h"
#include "engine/component/collider.h"
#include "engine/system/collision.h"
#include "engine/vec2d.h"
#include "extensions/gtest_exts.h"

#include <gtest/gtest.h>

using namespace ccsakura::components;
using ccsakura::vec2d;

TEST(AABBCollider, VsAABBNoCollision)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(aabb_collider({0, -4}, {1, 1}));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsAABBCollideOnYAxis)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(aabb_collider({0, -1.5}, {1, 1}));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, vec2d(0, -1));
}

TEST(AABBCollider, VsAABBCollideOnXAxis)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(aabb_collider({-1.3, 0}, {1, 1}));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.7);
    ASSERT_EQ(ab.normal, vec2d(-1, 0));
}

TEST(AABBCollider, VsAABBTouching)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(aabb_collider({2, 0}, {1, 1}));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0);
}

TEST(AABBCollider, VsCircleNoCollision)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(circle_collider({5, 5}, 1));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsCircleCollideNormally)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(circle_collider({0, 1.5}, 1));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, vec2d(0, 1));
}

TEST(AABBCollider, VsCircleOverlap)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(circle_collider({0.7, 0}, 0.3));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.6);
    ASSERT_EQ(ab.normal, vec2d(1, 0));
}

TEST(AABBCollider, VsCapsuleNoCollision)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(capsule_collider({-4, 0}, {-8, 1}, 1));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsCapsuleCollideOutside)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(capsule_collider({1.5, 0}, {1.5, 4}, 1));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, vec2d(1, 0));
}

TEST(AABBCollider, VsCapsuleOverlap)
{
    collider a(aabb_collider({0, 0}, {1, 1}));
    collider b(capsule_collider({-1, 0}, {-4, 0}, 1));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 1);
    ASSERT_EQ(ab.normal, vec2d(-1, 0));
}

TEST(OBBCollider, VsOBBNoCollision)
{
    collider a(obb_collider({0, 0}, {1, 1}, std::numbers::pi / 4));
    collider b(obb_collider({0, -4}, {1, 1}, std::numbers::pi / 4));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsOBBCollide)
{
    collider a(obb_collider({0, 0}, {1, 1}, std::numbers::pi / 4));
    collider b(obb_collider({1.5, 0}, {1, 1}, std::numbers::pi / 4));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);

    b.as<obb_collider>().center += ab.normal * ab.depth;
    ab = ccsakura::system::check_collisions(a, b);
    ASSERT_DOUBLE_EQ(ab.depth, 0);
}

TEST(OBBCollider, VsCircleNoCollision)
{
    collider a(obb_collider({0, 0}, {1, 1}, std::numbers::pi / 6));
    collider b(circle_collider({3, 3}, 0.5));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsCircleCollide)
{
    collider a(obb_collider({0, 0}, {1, 1}, std::numbers::pi / 6));
    collider b(circle_collider({0, 1}, 0.4));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);

    b.as<circle_collider>().center += ab.normal * (ab.depth + 0.001);
    ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsCapsuleNoCollision)
{
    collider a(obb_collider({0, 0}, {1, 1}, std::numbers::pi / 8));
    collider b(capsule_collider({4, 4}, {5, 5}, 1));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsCapsuleCollide)
{
    collider a(obb_collider({0, 0}, {1, 1}, std::numbers::pi / 8));
    collider b(capsule_collider({0.4, 0.4}, {2, 2}, 0.2));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);

    b.as<capsule_collider>().p1 += ab.normal * (ab.depth + 0.001);
    b.as<capsule_collider>().p2 += ab.normal * (ab.depth + 0.001);
    ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, VsCircleNoCollision)
{
    collider a(circle_collider({0, 0}, 1));
    collider b(circle_collider({-4, 4}, 1));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, VsCircleCollide)
{
    collider a(circle_collider({0, 0}, 1));
    collider b(circle_collider({0, -1}, 0.2));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.2);
    ASSERT_EQ(ab.normal, vec2d(0, -1));
}

TEST(CircleCollider, VsCapsuleNoCollision)
{
    collider a(circle_collider({0, 0}, 1));
    collider b(capsule_collider({-2, 0}, {-5, 0}, 0.5));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CircleCollider, VsCapsuleCollide)
{
    collider a(circle_collider({0, 0}, 1));
    collider b(capsule_collider({-1, 0}, {-5, 0}, 0.5));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, vec2d(-1, 0));
}

TEST(CapsuleCollider, VsCapsuleNoCollision)
{
    collider a(capsule_collider({0, 0}, {2, 0}, 1));
    collider b(capsule_collider({5, 0}, {8, 0}, 0.5));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_FALSE(ab.is_colliding);
}

TEST(CapsuleCollider, VsCapsuleCollide)
{
    collider a(capsule_collider({0, 0}, {2, 0}, 1));
    collider b(capsule_collider({3, 0}, {8, 0}, 0.5));

    ccsakura::collision ab = ccsakura::system::check_collisions(a, b);
    ASSERT_TRUE(ab.is_colliding);
    ASSERT_DOUBLE_EQ(ab.depth, 0.5);
    ASSERT_EQ(ab.normal, vec2d(1, 0));
}
