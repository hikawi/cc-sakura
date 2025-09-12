#include "engine/text.h"
#include "mocks/mock_font.h"
#include "mocks/mock_font_cache.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_surface.h"
#include "mocks/mock_texture.h"

#include "gmock/gmock.h"
#include <gtest/gtest.h>

TEST(Text, RendersCorrectly)
{
    using namespace testing;

    mock_font_cache cache;
    mock_renderer renderer;
    mock_font font;

    EXPECT_CALL(cache, BracketOp).Times(1).WillRepeatedly(ReturnRef(font));
    EXPECT_CALL(font, render_text_blended).Times(1).WillRepeatedly([]() { return std::make_unique<mock_surface>(); });
    EXPECT_CALL(renderer, create_texture(A<const sdl::isurface &>()))
        .Times(1)
        .WillRepeatedly([]() { return std::make_unique<mock_texture>(); });

    ccsakura::text text({ccsakura::typeface::unifont, 16}, "Hello World", cache);
    text.set_text("Hello");
    text.set_color(sdl::color(0, 0, 128, 255));
    text.set_position({25, 25});
    text.render(renderer);
}
