#include "common.h"

#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"

SDL_Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (SDL_Color){
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    };
}

SDL_FColor rgbaf(float r, float g, float b, float a)
{
    return (SDL_FColor){
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    };
}

uint64_t strhash(const char *str)
{
    SDL_assert(str != NULL);

    // A hashing function generally does a rolling polynomial, choose p and m.
    // The hash is sum of (s[i] * p^i) mod m
    const uint64_t p = 53;
    const uint64_t m = (uint64_t)1e9 + 9;

    uint64_t sum = 0;
    uint64_t pow = 1;
    for (uint32_t i = 0; i < SDL_strlen(str); i++)
    {
        sum = (sum + (uint64_t)str[i] * pow) % m;
        pow = (pow * p) % m;
    }

    return sum;
}
