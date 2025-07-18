// engine/macros.h
//
// A collection of macros that maybe commonly used.

#pragma once

#define COLOR(R, G, B, A)  ((SDL_Color){.r = R, .g = G, .b = B, .a = A})
#define COLORF(R, G, B, A) ((SDL_FColor){.r = R, .g = G, .b = B, .a = A})
