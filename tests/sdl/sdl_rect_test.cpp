#include "sdl/sdl_rect.h"

#include <gtest/gtest.h>

TEST(SDLPoint, CanConstructCorrectly)
{
    sdl::point p1;
    sdl::point p2(69, 69);
    sdl::point p3(sdl::fpoint(2.3f, 2.3f));

    EXPECT_EQ(p1.x, 0);
    EXPECT_EQ(p1.y, 0);
    EXPECT_EQ(p2.x, 69);
    EXPECT_EQ(p2.y, 69);
    EXPECT_EQ(p3.y, 2);
    EXPECT_EQ(p3.x, 2);
}

TEST(SDLFPoint, CanConstructCorrectly)
{
    sdl::fpoint p1;
    sdl::fpoint p2(69.5f, 30.5f);
    sdl::fpoint p3(sdl::point(2, 2));

    EXPECT_FLOAT_EQ(p1.x, 0);
    EXPECT_FLOAT_EQ(p1.y, 0);
    EXPECT_FLOAT_EQ(p2.x, 69.5);
    EXPECT_FLOAT_EQ(p2.y, 30.5);
    EXPECT_FLOAT_EQ(p3.y, 2);
    EXPECT_FLOAT_EQ(p3.x, 2);
}

TEST(SDLRect, CanConstructCorrectly)
{
    sdl::rect rect1(1, 2, 3, 4);
    sdl::rect rect2(sdl::frect(5.6f, 5.7f, 5.8f, 5.9f));

    EXPECT_EQ(rect1, sdl::rect(1, 2, 3, 4));
    EXPECT_EQ(rect2, sdl::rect(5, 5, 5, 5));
}

TEST(SDLFRect, CanConstructCorrectly)
{
    sdl::frect rect1(1.5f, 2.5f, 3.5f, 4.5f);
    sdl::frect rect2(sdl::rect(4, 5, 6, 7));

    EXPECT_EQ(rect1, sdl::frect(1.5f, 2.5f, 3.5f, 4.5f));
    EXPECT_EQ(rect2, sdl::frect(4, 5, 6, 7));
}
