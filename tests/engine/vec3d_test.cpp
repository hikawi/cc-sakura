#include "engine/vec3d.h"
#include "extensions/gtest_exts.h"

#include <cmath>
#include <gtest/gtest.h>
#include <tuple>

TEST(Vec3d, DefaultConstructorBuilds)
{
    ccsakura::vec3d vec;
    ASSERT_DOUBLE_EQ(vec.x, 0);
    ASSERT_DOUBLE_EQ(vec.y, 0);
    ASSERT_DOUBLE_EQ(vec.z, 0);
}

TEST(Vec3d, ComponentConstructorBuilds)
{
    ccsakura::vec3d vec(1, 69, -5);
    ASSERT_DOUBLE_EQ(vec.x, 1);
    ASSERT_DOUBLE_EQ(vec.y, 69);
    ASSERT_DOUBLE_EQ(vec.z, -5);
}

TEST(Vec3d, ArithmeticsAreCorrect)
{
    ccsakura::vec3d a(1, 2, 3);
    ccsakura::vec3d b(4, 5, 6);

    ASSERT_EQ(a + b, ccsakura::vec3d(5, 7, 9));
    ASSERT_EQ(a - b, ccsakura::vec3d(-3, -3, -3));
    ASSERT_EQ(a * -3, ccsakura::vec3d(-3, -6, -9));
    ASSERT_EQ(b / 2, ccsakura::vec3d(2, 2.5, 3));
    ASSERT_EQ(4 * a, ccsakura::vec3d(4, 8, 12));
    ASSERT_EQ(2 / b, ccsakura::vec3d(0.5, 0.4, 1.0 / 3.0));
}

TEST(Vec3d, AssignedArithmeticsAreCorrect)
{
    ccsakura::vec3d a(5, 5, 5);
    ccsakura::vec3d b(-1, 0, -1);
    ccsakura::vec3d c(3, 2, 1);
    ccsakura::vec3d d(2, 3, 4);

    a += ccsakura::vec3d(1, 1, 1);
    b -= ccsakura::vec3d(-1, 0, -1);
    c *= 5;
    d /= 2;

    ASSERT_EQ(a, ccsakura::vec3d(6, 6, 6));
    ASSERT_EQ(b, ccsakura::vec3d(0, 0, 0));
    ASSERT_EQ(c, ccsakura::vec3d(15, 10, 5));
    ASSERT_EQ(d, ccsakura::vec3d(1, 1.5, 2));
}

TEST(Vec3d, VectorOperationsAreCorrect)
{
    ccsakura::vec3d a(1, 2, 3);
    ccsakura::vec3d b(1, 1, 1);
    ccsakura::vec3d cross_prod = a.cross(b);

    ASSERT_DOUBLE_EQ(a.length_squared(), 14);
    ASSERT_DOUBLE_EQ(a.length(), std::sqrt(14));
    ASSERT_DOUBLE_EQ(a.dot(b), 6);
    ASSERT_DOUBLE_EQ(a.dot(cross_prod), 0);
    ASSERT_DOUBLE_EQ(b.dot(cross_prod), 0);
    ASSERT_DOUBLE_EQ(a.normalized().length_squared(), 1);
    ASSERT_DOUBLE_EQ(a.normalized().dot(a), a.length());
}

TEST(Vec3d, ClosestPointOnSegmentDegenerateCase)
{
    ccsakura::vec3d a(1, 1, 1);
    ccsakura::vec3d b(1, 1, 1);
    ccsakura::vec3d p(5, 5, 5);

    ccsakura::vec3d q = ccsakura::closest_point_on_segment(a, b, p);
    ASSERT_EQ(q, a);
}

TEST(Vec3d, ClosestPointOnSegmentEndpointCase)
{
    ccsakura::vec3d a(1, 1, 1);
    ccsakura::vec3d b(-1, -1, -1);
    ccsakura::vec3d p(5, 5, 5);

    ccsakura::vec3d q = ccsakura::closest_point_on_segment(a, b, p);
    ASSERT_EQ(q, a);
}

TEST(Vec3d, ClosestPointOnSegmentParallelCase)
{
    ccsakura::vec3d a(0, 0, 0);
    ccsakura::vec3d b(0, 0, 5);
    ccsakura::vec3d p(2, 0, 2.5);

    ccsakura::vec3d q = ccsakura::closest_point_on_segment(a, b, p);
    ASSERT_EQ(q, ccsakura::vec3d(0, 0, 2.5));
}

TEST(Vec3d, ClosestPointsBetweenSegmentsDegenerateCase)
{
    ccsakura::vec3d a(0, 0, 0);
    ccsakura::vec3d b(5, 5, 5);

    auto [p, q] = ccsakura::closest_points_between_segments(a, a, b, b);
    ASSERT_EQ(p, a);
    ASSERT_EQ(q, b);
}

TEST(Vec3d, ClosestPointsBetweenSegmentsHalfDegenerateCase)
{
    ccsakura::vec3d a(0, 0, 0);
    ccsakura::vec3d b(0, 0, 5);
    ccsakura::vec3d c(0, 0, 6);
    ccsakura::vec3d d(0, 0, 8);

    auto [p, q] = ccsakura::closest_points_between_segments(a, a, b, c);
    ASSERT_EQ(p, a);
    ASSERT_EQ(q, b);

    std::tie(p, q) = ccsakura::closest_points_between_segments(b, b, c, d);
    ASSERT_EQ(p, b);
    ASSERT_EQ(q, c);
}

TEST(Vec3d, ClosestPointsBetweenSegmentsParallelCase)
{
    ccsakura::vec3d a(5, 5, 5);
    ccsakura::vec3d b(6, 6, 6);
    ccsakura::vec3d c(7, 7, 7);
    ccsakura::vec3d d(8, 8, 8);

    auto [p, q] = ccsakura::closest_points_between_segments(a, b, c, d);
    ASSERT_EQ(p, b);
    ASSERT_EQ(q, c);
}

TEST(Vec3d, ClosestPointsBetweenSegmentsNormalCase)
{
    ccsakura::vec3d a(0, 0, 0);
    ccsakura::vec3d b(1, 0, 0);
    ccsakura::vec3d c(0, 1, 1);
    ccsakura::vec3d d(0, 2, 1);

    auto [p, q] = ccsakura::closest_points_between_segments(a, b, c, d);
    ASSERT_EQ(p, ccsakura::vec3d(0, 0, 0));
    ASSERT_EQ(q, ccsakura::vec3d(0, 1, 1));
}
