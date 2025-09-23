#include "engine/sprite.h"
#include "sdl/sdl_render.h"

#include "gmock/gmock.h"

class mock_sprite : public ccsakura::isprite
{
  public:
    MOCK_METHOD(void, render, (const sdl::irenderer &, const ccsakura::vec2d, const ccsakura::render_origin),
                (const, noexcept, override));
};
