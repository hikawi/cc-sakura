#include "engine/colllision.h"
#include "engine/vec3d.h"
#include "extensions/gtest_exts.h"

#include <gtest/gtest.h>
#include <stdexcept>

constexpr std::array<ccsakura::vec3d, 3> identity_axes = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

TEST(AABBCollider, FailsWithNegativeDims)
{
    EXPECT_THROW(ccsakura::aabb_collider a({0, 0, 0}, {-1, -1, 2}), std::invalid_argument);
}

TEST(OBBCollider, FailsWithNonOrthogonalAxes)
{
    std::array<ccsakura::vec3d, 3> axes({{1, 0, 0}, {0, 1, 1}, {0, 0, 1}});
    EXPECT_THROW(ccsakura::obb_collider a({0, 0, 0}, axes, {2, 2, 2}), std::invalid_argument);
}

TEST(OBBCollider, FailsWithNegativeDims)
{
    EXPECT_THROW(ccsakura::obb_collider a({0, 0, 0}, identity_axes, {-2, -3, 2.5}), std::invalid_argument);
}

TEST(SphereCollider, FailsWithNegativeRadius)
{
    EXPECT_THROW(ccsakura::sphere_collider({0, 0, 0}, -3), std::invalid_argument);
}

TEST(CapsuleCollider, FailsWithNegativeRadius)
{
    EXPECT_THROW(ccsakura::capsule_collider({0, 0, 0}, {0, 0, 1}, -3), std::invalid_argument);
}

TEST(AABBCollider, VsAABBColliderNoCollisions)
{
    ccsakura::aabb_collider a({0, 0, 0}, {1, 1, 1});
    ccsakura::aabb_collider b({0, 0, 3}, {1, 1, 1});

    ccsakura::collision ab = a.collides(b);
    EXPECT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsAABBColliderCollide)
{
    ccsakura::aabb_collider a({0, 0, 0}, {1, 1, 1});
    ccsakura::aabb_collider b({0, 0, 0.8}, {1, 1, 1});

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 0.2);
    EXPECT_EQ(ab.normal, ccsakura::vec3d(0, 0, 1));
}

TEST(AABBCollider, VsSphereColliderNoCollisions)
{
    ccsakura::aabb_collider a({0, 0, 0}, {1, 1, 1});
    ccsakura::sphere_collider b({0, 0, 3}, 1);

    ccsakura::collision ab = a.collides(b);
    EXPECT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsSphereColliderCollide)
{
    ccsakura::aabb_collider a({0, 0, 0}, {1, 1, 1});
    ccsakura::sphere_collider b({0, 0, -1}, 0.6);

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 0.1);
    EXPECT_EQ(ab.normal, ccsakura::vec3d(0, 0, -1));
}

TEST(AABBCollider, VsSphereColliderFullOverlap)
{
    ccsakura::aabb_collider a({0, 0, 0}, {1, 1, 1});
    ccsakura::sphere_collider b({0, 0, 0}, 0.3);

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 0.8);
}

TEST(AABBCollider, VsCapsuleColliderNoCollisions)
{
    ccsakura::aabb_collider a({0, 0, 0}, {1, 1, 1});
    ccsakura::capsule_collider b({2, 0, 0}, {5, 0, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    EXPECT_FALSE(ab.is_colliding);
}

TEST(AABBCollider, VsCapsuleColliderCollide)
{
    ccsakura::aabb_collider a({0, 0, 0}, {1, 1, 1});
    ccsakura::capsule_collider b({1, 0, 0}, {5, 0, 0}, 0.6);

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 0.1);
    EXPECT_EQ(ab.normal, ccsakura::vec3d(1, 0, 0));
}

TEST(OBBCollider, VsOBBColliderNoCollisions)
{
    ccsakura::obb_collider a{{0, 0, 0}, identity_axes, {2, 2, 2}};
    ccsakura::obb_collider b{{2, 0, 0}, identity_axes, {1, 1, 1}};

    ccsakura::collision ab = a.collides(b);
    EXPECT_FALSE(ab.is_colliding);
}

TEST(OBBCollider, VsOBBColliderCollide)
{
    ccsakura::obb_collider a{{0, 0, 0}, identity_axes, {2, 2, 2}};
    ccsakura::obb_collider b{{-1.5, 0, 0}, identity_axes, {2, 2, 2}};

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 0.5);
    EXPECT_EQ(ab.normal, ccsakura::vec3d(-1, 0, 0));
}

TEST(SphereCollider, VsSphereColliderNoCollisions)
{
    ccsakura::sphere_collider a({0, 0, 0}, 1);
    ccsakura::sphere_collider b({0, 3, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    EXPECT_FALSE(ab.is_colliding);
}

TEST(SphereCollider, VsSphereColliderCollide)
{
    ccsakura::sphere_collider a({0, 0, 0}, 1);
    ccsakura::sphere_collider b({0, -1.5, 0}, 1);

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 0.5);
    EXPECT_EQ(ab.normal, ccsakura::vec3d(0, -1, 0));
}

TEST(SphereCollider, VsSphereColliderFullOverlap)
{
    ccsakura::sphere_collider a({0, 0, 0}, 1);
    ccsakura::sphere_collider b({0, 0, 0}, 2);

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 3);
    EXPECT_EQ(ab.normal, ccsakura::vec3d(0, 0, 1));
}

TEST(CapsuleCollider, VsCapsuleColliderNoCollisions)
{
    ccsakura::capsule_collider a({0, 0, 0}, {0, 0, 5}, 1);
    ccsakura::capsule_collider b({4, 4, 0}, {4, 4, 5}, 1);

    ccsakura::collision ab = a.collides(b);
    EXPECT_FALSE(ab.is_colliding);
}

TEST(CapsuleCollider, VsCapsuleColliderCollide)
{
    ccsakura::capsule_collider a({0, 0, 0}, {0, 0, 5}, 1);
    ccsakura::capsule_collider b({0, 1.5, 0}, {0, 1.5, 5}, 1);

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 0.5);
    EXPECT_EQ(ab.normal, ccsakura::vec3d(0, 1, 0));
}

TEST(CapsuleCollider, VsCapsuleColliderFullOverlap)
{
    ccsakura::capsule_collider a({0, 0, 0}, {0, 0, 5}, 1);
    ccsakura::capsule_collider b({0, 0, 5}, {0, 0, 10}, 1);

    ccsakura::collision ab = a.collides(b);
    EXPECT_TRUE(ab.is_colliding);
    EXPECT_DOUBLE_EQ(ab.depth, 2);
}
