#include "engine/sprite.h"

#include "gmock/gmock.h"

class mock_sprite_cache : public ccsakura::isprite_cache
{
  public:
    ccsakura::isprite &operator[](std::string name) override
    {
        return Get(name);
    }

    MOCK_METHOD(ccsakura::isprite &, Get, (std::string), ());
    MOCK_METHOD(void, insert, (const std::string, std::unique_ptr<ccsakura::isprite>), (noexcept, override));
    MOCK_METHOD(bool, has, (const std::string), (const, noexcept, override));
    MOCK_METHOD(void, clear, (), (noexcept, override));
};
