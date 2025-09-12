#include "sdl/sdl_pixels.h"

#include "gtest/gtest.h"
#include <gtest/gtest.h>

TEST(FColor, ConvertsToColor)
{
    sdl::fcolor fcolor(0.5, 0.5, 0.5, 1);
    sdl::color color = fcolor;
    ASSERT_EQ(color.r, 128);
    ASSERT_EQ(color.g, 128);
    ASSERT_EQ(color.b, 128);
    ASSERT_EQ(color.a, 255);
}

TEST(FColor, GetsClamped)
{
    sdl::fcolor fcolor(2, 2, -1, 500);
    ASSERT_FLOAT_EQ(fcolor.r, 1);
    ASSERT_FLOAT_EQ(fcolor.g, 1);
    ASSERT_FLOAT_EQ(fcolor.b, 0);
    ASSERT_FLOAT_EQ(fcolor.a, 1);
}

TEST(Color, ConvertsToFColor)
{
    sdl::color color(255, 255, 1, 140);
    sdl::fcolor fcolor = color;
    ASSERT_FLOAT_EQ(fcolor.r, 1);
    ASSERT_FLOAT_EQ(fcolor.g, 1);
    ASSERT_FLOAT_EQ(fcolor.b, 1 / 255.0f);
    ASSERT_FLOAT_EQ(fcolor.a, 140 / 255.0f);
}
