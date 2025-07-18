#include "misc/mathex.h"

#include "SDL3/SDL_stdinc.h"

bool feq(double a, double b)
{
    return SDL_fabs(a - b) < (double)SDL_FLT_EPSILON;
}

bool feqf(float a, float b)
{
    return SDL_fabsf(a - b) < SDL_FLT_EPSILON;
}
