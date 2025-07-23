#include "common.h"

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
