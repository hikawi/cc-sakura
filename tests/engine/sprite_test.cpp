#include "engine/sprite.h"
#include "mocks/mock_sprite.h"
#include "mocks/mock_sprite_cache.h"
#include "mocks/mock_storage.h"

#include <cstdlib>
#include <gtest/gtest.h>

TEST(Sprite, ThrowsIfNoDependenciesAreSet)
{
    ASSERT_ANY_THROW(ccsakura::sprite::named("wee"));
}
