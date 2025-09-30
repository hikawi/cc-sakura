#include "engine/vec2d.h"
#include "extensions/gtest_exts.h"

#include <cmath>
#include <gtest/gtest.h>
#include <stdexcept>

TEST(Vec2d, ConstructorWorksCorrectly)
{
    ccsakura::vec2d a;
    ccsakura::vec2d b(5, -5);

    ASSERT_DOUBLE_EQ(a.x, 0);
    ASSERT_DOUBLE_EQ(a.y, 0);
    ASSERT_DOUBLE_EQ(b.x, 5);
    ASSERT_DOUBLE_EQ(b.y, -5);
}

TEST(Vec2d, ArithmeticOperationsAreCorrect)
{
    ccsakura::vec2d a(1, 2);
    ccsakura::vec2d b(4, 5);

    ASSERT_EQ(a + b, ccsakura::vec2d(5, 7));
    ASSERT_EQ(a - b, ccsakura::vec2d(-3, -3));
    ASSERT_EQ(a * 2, ccsakura::vec2d(2, 4));
    ASSERT_EQ(3 * b, ccsakura::vec2d(12, 15));
    ASSERT_EQ(a / 3, ccsakura::vec2d(1 / 3.0, 2 / 3.0));
    ASSERT_THROW(a / 0, std::invalid_argument);
}

TEST(Vec2d, AssignedArithmeticOperationsAreCorrect)
{
    ccsakura::vec2d a(1, 2);
    ccsakura::vec2d b(4, 5);

    a += b;
    ASSERT_EQ(a, ccsakura::vec2d(5, 7));
    a -= ccsakura::vec2d(5, 5);
    ASSERT_EQ(a, ccsakura::vec2d(0, 2));
    a *= -3;
    ASSERT_EQ(a, ccsakura::vec2d(0, -6));
    a /= -2;
    ASSERT_EQ(a, ccsakura::vec2d(0, 3));
}

TEST(Vec2d, NegationIsCorrect)
{
    ccsakura::vec2d a(69, 420);

    ASSERT_EQ(-a, ccsakura::vec2d(-69, -420));
}

TEST(Vec2d, UnaryFunctionsAreCorrect)
{
    ccsakura::vec2d a(4, -5);

    ASSERT_EQ(a.abs(), ccsakura::vec2d(4, 5));
    ASSERT_EQ(a.length_squared(), 41);
    ASSERT_EQ(a.length(), std::sqrt(41));
    ASSERT_DOUBLE_EQ(a.normalized().length(), 1);
    ASSERT_NEAR(a.orthogonal().dot(a), 0, 1e-9);
    ASSERT_EQ(-a, a.rotated(std::numbers::pi));
}

TEST(Vec2d, BinaryFunctionsAreCorrect)
{
    ccsakura::vec2d a(1, 2);
    ccsakura::vec2d b(-6, 3);

    ASSERT_EQ(a.min(b), ccsakura::vec2d(-6, 2));
    ASSERT_EQ(a.max(b), ccsakura::vec2d(1, 3));
    ASSERT_DOUBLE_EQ(a.dot(b), 0);
    ASSERT_EQ(a.distance_squared(b), 50);
}
