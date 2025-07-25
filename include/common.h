/**
 * \file common.h
 *
 * Commonly used utilities and structures for all sources.
 */

#pragma once

#include "SDL3/SDL_pixels.h"

#include <stdint.h>

/**
 * Type definition for a hash function.
 */
typedef uint64_t (*HashFunction)(const void *);

/**
 * Type definition for a compare function.
 */
typedef int (*CompareFunction)(const void *, const void *);

/**
 * Constructs a color based on integer RGBA values.
 *
 * \param r red value
 * \param g green value
 * \param b blue value
 * \param a alpha value
 *
 * \returns an integer-based `SDL_Color`
 * \see rgbaf
 */
SDL_Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * Constructs a color based on floating points RGBA values.
 *
 * \param r red value
 * \param g green value
 * \param b blue value
 * \param a alpha value
 *
 * \returns a float-based `SDL_FColor`
 * \see rgba
 */
SDL_FColor rgbaf(float r, float g, float b, float a);

/**
 * Hashs a string.
 *
 * \param str the string to hash
 * \returns the hash value of a string
 */
uint64_t strhash(const char *str);
