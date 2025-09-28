#include "engine/render.h"
#include "engine/sprite.h"

#include "gmock/gmock.h"

class mock_sprite : public ccsakura::isprite
{
  public:
    MOCK_METHOD(void, render, (const sdl::irenderer &, const sdl::fpoint, const ccsakura::render_origin),
                (const, noexcept, override));
};
