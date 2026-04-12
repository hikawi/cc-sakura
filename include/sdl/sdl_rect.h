/**
 * \file sdl/sdl_rect.h
 *
 * Simple wrappers for SDL rectangles with conversions towards each other.
 */

#pragma once

#include <SDL3/SDL_rect.h>

namespace sdl
{

/**
 * Represents a single point on the 2D screen, using integers as components.
 */
struct point;

/**
 * Represents a single point on the 2D screen, using floats as components.
 */
struct fpoint;

struct point
{
    int x; ///< the x coord of a point in SDL
    int y; ///< the y coord of a point in SDL

    /**
     * Constructs a zero point.
     */
    point() noexcept;

    /**
     * Creates a point with defined components.
     *
     * \param x the x component
     * \param y the y component
     */
    point(const int x, const int y) noexcept;

    /**
     * Converts a floating-point point to an integer point.
     *
     * \param other the other point to convert
     */
    point(const fpoint &other) noexcept;

    /**
     * Checks if two points are equal.
     *
     * \param other the other point to check against
     * \returns true if two points are equal
     */
    bool operator==(const point &other) const noexcept;

    /**
     * Checks if two points are inequal.
     *
     * \param other the other point to check against
     * \returns true if two points are inequal
     */
    bool operator!=(const point &other) const noexcept;

    /**
     * Retrieves the SDL version of the point.
     *
     * \returns the sdl point
     */
    SDL_Point to_sdl() const noexcept;
};

struct fpoint
{
    float x; ///< the x coord of a point in SDL
    float y; ///< the y coord of a point in SDL

    /**
     * Constructs a zero point at origin.
     */
    fpoint() noexcept;

    /**
     * Constructs a floating-point point (lol) with defined components.
     *
     * \param x the x component
     * \param y the y component
     */
    fpoint(const float x, const float y) noexcept;

    /**
     * Constructs a floating-point point from an integer point.
     *
     * \param other the other point
     */
    fpoint(const point &other) noexcept;

    /**
     * Checks if two points are equal.
     *
     * \param other the other point to check against
     * \returns true if two points are equal
     */
    bool operator==(const fpoint &other) const noexcept;

    /**
     * Checks if two points are inequal.
     *
     * \param other the other point to check against
     * \returns true if two points are inequal
     */
    bool operator!=(const fpoint &other) const noexcept;

    /**
     * Converts this floating point into a SDL-version.
     *
     * \returns the sdl fpoint
     */
    SDL_FPoint to_sdl() const noexcept;
};

/**
 * Represents a simple rectangle on the screen, using integers for values.
 */
struct rect;

/**
 * Represents a simple rectangle on the screen, using floating points for values.
 */
struct frect;

struct rect
{
    int x; ///< the top left corner of the rectangle
    int y; ///< the top right corner of the rectangle
    int w; ///< the full width extent of the rectangle
    int h; ///< the full height extent of the rectangle

    /**
     * Constructs an all-zero rectangle.
     */
    rect() noexcept;

    /**
     * Constructs a rectangle with pre-configured values.
     *
     * \param x the x component
     * \param y the y component
     * \param w the w component
     * \param h the h component
     */
    rect(const int x, const int y, const int w, const int h) noexcept;

    /**
     * Converts a floating rectangle to an integer rectangle.
     *
     * \param frect the floating-based rectangle to convert
     */
    rect(const frect &frect) noexcept;

    /**
     * Checks if two rectangles are equal.
     *
     * \param other the other rectangle to check against
     * \returns true if two rectangles are equal
     */
    bool operator==(const rect &other) const noexcept;

    /**
     * Checks if two rectangles are not equal.
     *
     * \param other the other rectangle to check against
     * \returns true if two rectangles are not equal
     */
    bool operator!=(const rect &other) const noexcept;
};

struct frect
{
    float x; ///< the top left corner of the rectangle
    float y; ///< the top right corner of the rectangle
    float w; ///< the full width extent of the rectangle
    float h; ///< the full height extent of the rectangle

    /**
     * Constructs an all-zero floating-based rectangle.
     */
    frect() noexcept;

    /**
     * Constructs a floating-based rectangle with pre-configured values.
     *
     * \param x the x component
     * \param y the y component
     * \param w the w component
     * \param h the h component
     */
    frect(const float x, const float y, const float w, const float h) noexcept;

    /**
     * Converts an integer rectangle to floating-based rectangle.
     *
     * \param rect the integer rectangle to convert
     */
    frect(const rect &rect) noexcept;

    /**
     * Checks if two rectangles are equal.
     *
     * \param other the other rectangle to check against
     * \returns true if two rectangles are equal
     */
    bool operator==(const frect &other) const noexcept;

    /**
     * Checks if two rectangles are not equal.
     *
     * \param other the other rectangle to check against
     * \returns true if two rectangles are not equal
     */
    bool operator!=(const frect &other) const noexcept;
};

} // namespace sdl
