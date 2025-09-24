#include "engine/sprite.h"
#include "mocks/mock_renderer.h"
#include "mocks/mock_sprite.h"
#include "mocks/mock_sprite_cache.h"
#include "mocks/mock_storage.h"
#include "sdl/sdl_storage.h"

#include "gmock/gmock.h"
#include <bit>
#include <cstddef>
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <SDL3/SDL_endian.h>
#include <stdexcept>
#include <vector>

namespace
{

void write_to_byte_vec(std::vector<std::byte> &bytes, void *ptr, size_t len)
{
    std::byte *bytes_ptr = reinterpret_cast<std::byte *>(ptr);
    bytes.insert(bytes.end(), bytes_ptr, bytes_ptr + len);
}

void write_u32le(std::vector<std::byte> &bytes, uint32_t val)
{
    if (std::endian::native != std::endian::little)
    {
        val = std::byteswap(val);
    }
    std::byte *uint32_bytes = reinterpret_cast<std::byte *>(&val);
    write_to_byte_vec(bytes, uint32_bytes, sizeof(uint32_t));
}

std::vector<std::byte> generate_bad_version_sprite()
{
    std::vector<std::byte> bytes;
    write_u32le(bytes, 0);
    return bytes;
}

} // namespace

TEST(Sprite, ThrowsIfNoDependenciesAreSet)
{
    ASSERT_ANY_THROW(ccsakura::sprite::named("wee"));
}

TEST(Sprite, FailsImmediatelyIfCantOpenFile)
{
    using namespace ::testing;

    mock_sprite_cache cache;
    mock_storage *storage = new mock_storage;
    mock_renderer renderer;

    ASSERT_TRUE(storage != nullptr);

    ccsakura::sprite::use_cache(cache);
    ccsakura::sprite::use_renderer(renderer);
    ccsakura::sprite::use_storage_opener([&]() { return std::unique_ptr<sdl::istorage>(storage); });

    EXPECT_CALL(cache, has).Times(1).WillOnce(Return(false));
    EXPECT_CALL(cache, Get).Times(0);

    EXPECT_CALL(*storage, wait_until_ready).Times(1);
    EXPECT_CALL(*storage, read_file).Times(1).WillOnce(Return(std::nullopt));

    EXPECT_THROW(ccsakura::sprite::named("test"), std::runtime_error);
}

TEST(Sprite, UsesTheCacheIfAvailable)
{
    using namespace ::testing;

    mock_sprite_cache cache;
    mock_storage *storage = new mock_storage;
    mock_renderer renderer;
    mock_sprite sprite;

    ASSERT_TRUE(storage != nullptr);

    ccsakura::sprite::use_cache(cache);
    ccsakura::sprite::use_renderer(renderer);
    ccsakura::sprite::use_storage_opener([&]() { return std::unique_ptr<sdl::istorage>(storage); });

    EXPECT_CALL(cache, has("test")).Times(1).WillOnce(Return(true));
    EXPECT_CALL(cache, Get).Times(1).WillOnce(ReturnRef(sprite));

    ccsakura::isprite &returned_spr = ccsakura::sprite::named("test");
    ASSERT_EQ(&returned_spr, &sprite);
}

TEST(Sprite, FailsOnIncorrectSpriteVersion)
{
    using namespace ::testing;

    mock_sprite_cache cache;
    mock_storage *storage = new mock_storage;
    mock_renderer renderer;

    ASSERT_TRUE(storage != nullptr);

    ccsakura::sprite::use_cache(cache);
    ccsakura::sprite::use_renderer(renderer);
    ccsakura::sprite::use_storage_opener([&]() { return std::unique_ptr<sdl::istorage>(storage); });

    EXPECT_CALL(cache, has("test")).Times(1).WillOnce(Return(false));
    EXPECT_CALL(cache, Get).Times(0);

    EXPECT_CALL(*storage, wait_until_ready).Times(1);
    EXPECT_CALL(*storage, read_file).Times(1).WillOnce(Return(std::optional(generate_bad_version_sprite())));

    ASSERT_THROW(ccsakura::sprite::named("test"), std::runtime_error);
}
