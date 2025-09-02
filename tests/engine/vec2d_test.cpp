#include "engine/vec2d.h"
#include "utils.h"

#include <gtest/gtest.h>
#include <stdexcept>

using ccsakura::vec2d;

namespace ccsakura
{

void PrintTo(const vec2d &v, std::ostream *os)
{
    *os << "(" << v.x << "," << v.y << ")";
}

} // namespace ccsakura

TEST(Vec2d, ConstructorsInitializeCorrect)
{
    ASSERT_EQ(vec2d(), vec2d::zero());
    ASSERT_EQ(vec2d(1, 1), vec2d::one());
    ASSERT_EQ(vec2d(1, 0), vec2d::unit_x());
    ASSERT_EQ(vec2d(0, 1), vec2d::unit_y());
}

TEST(Vec2d, SimpleMathOperationsAreCorrect)
{
    ASSERT_EQ(vec2d(1, 1) + vec2d(2, 2), vec2d(3, 3));
    ASSERT_EQ(vec2d(1, 1) - vec2d(2, 2), vec2d(-1, -1));
    ASSERT_EQ(vec2d(1, 1) * 3, vec2d(3, 3));
    ASSERT_EQ(5 * vec2d(1, 1), vec2d(5, 5));
    ASSERT_EQ(vec2d(1, 1) / 3, vec2d(1.0 / 3, 1.0 / 3));
    ASSERT_EQ(-vec2d(69, 420), vec2d(-69, -420));
}

TEST(Vec2d, InplaceMathOperationsAreCorrect)
{
    vec2d vec(1, 1);

    vec += vec2d(2, 2);
    ASSERT_EQ(vec, vec2d(3, 3));

    vec -= vec2d(5, 5);
    ASSERT_EQ(vec, vec2d(-2, -2));

    vec *= 3;
    ASSERT_EQ(vec, vec2d(-6, -6));

    vec /= -6;
    ASSERT_EQ(vec, vec2d(1, 1));
}

TEST(Vec2d, HandlesDivisionByZero)
{
    ASSERT_THROW(vec2d(1, 1) / 0, std::invalid_argument);
    ASSERT_THROW(
        {
            vec2d vec(1, 1);
            vec /= 0;
        },
        std::invalid_argument);
}

TEST(Vec2d, ComparesAtEpsilonLevel)
{
    ASSERT_EQ(vec2d(0.1 + 0.2, 0.1 + 0.2), vec2d(0.3, 0.3));
    ASSERT_NE(vec2d(0.1 + 0.2 + ccsakura::epsilon, 0.1 + 0.2 + ccsakura::epsilon), vec2d(0.3, 0.3));
}

TEST(Vec2d, MagnitudeCalculationsAreCorrect)
{
    vec2d vec(2, 2);
    ASSERT_DOUBLE_EQ(vec.length_squared(), 8);
    ASSERT_DOUBLE_EQ(vec.length(), std::sqrt(8));

    vec2d vecb(5, 5);
    ASSERT_DOUBLE_EQ(vec.distance_squared(vecb), 18);
    ASSERT_DOUBLE_EQ(vec.distance(vecb), std::sqrt(18));
}

TEST(Vec2d, CanNormalize)
{
    vec2d vec(3, 4);
    ASSERT_DOUBLE_EQ(vec.length(), 5);

    vec2d norm = vec.normalized();
    ASSERT_DOUBLE_EQ(norm.length(), 1);

    ASSERT_EQ(vec2d::zero().normalized(), vec2d::zero());
}

TEST(Vec2d, ComputesAlgorithmsCorrectly)
{
    vec2d a(1, 2), b(3, 4);
    ASSERT_DOUBLE_EQ(a.dot(b), 11);
    ASSERT_EQ(a.rotated(std::numbers::pi), -a);
    ASSERT_EQ(a.rotated(std::numbers::pi / 2), a.perpendicular());
    ASSERT_EQ(a.rotated(-std::numbers::pi / 2), -a.perpendicular());
    ASSERT_EQ(vec2d::zero().lerp(vec2d::one(), 0.5), vec2d(0.5, 0.5));
}
