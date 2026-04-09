#include "engine/render.h"
#include "engine/sprite.h"

#include "gmock/gmock.h"

class mock_sprite : public ccsakura::isprite
{
  public:
    MOCK_METHOD(void, render, (const sdl::irenderer &, const sdl::fpoint, const ccsakura::render_origin, uint32_t),
                (const, noexcept, override));
    MOCK_METHOD(sdl::render_texture_options, render_options, (const sdl::irenderer &), (const, noexcept, override));
    MOCK_METHOD(const ccsakura::sprite_frame &, frame, (uint32_t), (const, noexcept, override));
    MOCK_METHOD(const ccsakura::sprite_frame_tag &, frame_tag, (const std::string &), (const, noexcept, override));
    MOCK_METHOD(uint32_t, frame_count, (), (const, noexcept, override));
};
