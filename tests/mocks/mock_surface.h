#include "sdl/sdl_surface.h"

#include <gmock/gmock.h>

class mock_surface : public sdl::isurface
{
  public:
    MOCK_METHOD(SDL_Surface *, get, (), (const, noexcept, override));
};
