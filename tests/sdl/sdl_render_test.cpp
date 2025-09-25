#include "engine/sprite.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_texture.h"
#include "sdl/sdl_render.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <gtest/gtest.h>

TEST(RenderGeometryBuilder, BuildCorrectly)
{
    using namespace ::testing;

    mock_renderer renderer;

    sdl::vertex v1({30, 40}, {0, 0, 0, 0});
    sdl::vertex v2({300, 400}, {0, 0, 0, 255}, {50, 50});

    sdl::render_geometry_options options(renderer);
    options.connect(4, 5, 9);
    options.connect(1, 2, 3);
    options.add_vertex(v1);
    options.add_vertex(v2);

    EXPECT_CALL(renderer,
                render_geometry(AllOf(Field(&sdl::render_geometry_options::texture, IsNull()),
                                      Field(&sdl::render_geometry_options::indices, ElementsAre(4, 5, 9, 1, 2, 3)),
                                      Field(&sdl::render_geometry_options::vertices, ElementsAre(v1, v2)))))
        .Times(1);

    options.render();
}
