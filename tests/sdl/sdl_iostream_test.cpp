#include "sdl/sdl_iostream.h"

#include <cstddef>
#include <cstring>
#include <gtest/gtest.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_properties.h>
#include <vector>

TEST(IOStream, CanWriteToDynamicMemory)
{
    sdl::iostream io;

    uint64_t num1 = 69;
    uint32_t num2 = 420;
    uint32_t num3 = 7749;

    io.write_u64_le(num1);
    io.write_u32_le(num2);
    io.write_u32_le(num3);
    ASSERT_EQ(SDL_GetIOSize(io.get()), 16);
}

TEST(IOStream, CanReadFromMemory)
{
    std::vector<std::byte> mem(16);

    uint64_t num1 = 69;
    uint32_t num2 = 420;
    uint32_t num3 = 7749;

    SDL_IOStream *sdl_io = SDL_IOFromMem(mem.data(), 16);
    SDL_WriteU64LE(sdl_io, num1);
    SDL_WriteU32LE(sdl_io, num2);
    SDL_WriteU32LE(sdl_io, num3);
    SDL_CloseIO(sdl_io);

    uint64_t val1 = 0;
    uint32_t val2 = 0;
    uint32_t val3 = 0;

    sdl::iostream io(mem);
    io.read_u64_le(val1);
    io.read_u32_le(val2);
    io.read_u32_le(val3);

    ASSERT_EQ(val1, num1);
    ASSERT_EQ(val2, num2);
    ASSERT_EQ(val3, num3);
}
