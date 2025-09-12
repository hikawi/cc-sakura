#include "engine/render.h"
#include "sdl/sdl_rect.h"

#include <gtest/gtest.h>

TEST(RenderOriginShift, TopLeftIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::top_left);
    ASSERT_FLOAT_EQ(rect.x, 0);
    ASSERT_FLOAT_EQ(rect.y, 0);
}

TEST(RenderOriginShift, TopCenterIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::top_center);
    ASSERT_FLOAT_EQ(rect.x, -100);
    ASSERT_FLOAT_EQ(rect.y, 0);
}

TEST(RenderOriginShift, TopRightIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::top_right);
    ASSERT_FLOAT_EQ(rect.x, -200);
    ASSERT_FLOAT_EQ(rect.y, 0);
}

TEST(RenderOriginShift, MiddleLeftIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::middle_left);
    ASSERT_FLOAT_EQ(rect.x, 0);
    ASSERT_FLOAT_EQ(rect.y, -100);
}

TEST(RenderOriginShift, MiddleCenterIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::middle_center);
    ASSERT_FLOAT_EQ(rect.x, -100);
    ASSERT_FLOAT_EQ(rect.y, -100);
}

TEST(RenderOriginShift, MiddleRightIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::middle_right);
    ASSERT_FLOAT_EQ(rect.x, -200);
    ASSERT_FLOAT_EQ(rect.y, -100);
}

TEST(RenderOriginShift, BottomLeftIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::bottom_left);
    ASSERT_FLOAT_EQ(rect.x, 0);
    ASSERT_FLOAT_EQ(rect.y, -200);
}

TEST(RenderOriginShift, BottomCenterIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::bottom_center);
    ASSERT_FLOAT_EQ(rect.x, -100);
    ASSERT_FLOAT_EQ(rect.y, -200);
}

TEST(RenderOriginShift, BottomRightIsCorrect)
{
    sdl::frect rect(0, 0, 200, 200);
    ccsakura::shift_origin(rect, ccsakura::render_origin::bottom_right);
    ASSERT_FLOAT_EQ(rect.x, -200);
    ASSERT_FLOAT_EQ(rect.y, -200);
}
