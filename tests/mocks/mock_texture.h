#include "sdl/sdl_render.h"

#include "gmock/gmock.h"

class mock_texture : public sdl::itexture
{
  public:
    MOCK_METHOD(SDL_Texture *, get, (), (const, noexcept, override));
    MOCK_METHOD(sdl::blend_mode, get_blend_mode, (), (const, noexcept, override));
    MOCK_METHOD(void, set_blend_mode, (const sdl::blend_mode), (const, noexcept, override));
    MOCK_METHOD(sdl::scale_mode, get_scale_mode, (), (const, noexcept, override));
    MOCK_METHOD(void, set_scale_mode, (const sdl::scale_mode), (const, noexcept, override));
};
