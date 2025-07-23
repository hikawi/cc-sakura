/**
 * \file engine/text.h
 *
 * The engine's text rendering module. This is used to draw various texts on surfaces with multiple variants of font
 * faces.
 */

#pragma once

#include "app.h"
#include "engine/renderer.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <stdbool.h>

/**
 * Represents a font face to be loaded.
 */
typedef enum
{
    /**
     * Main font face of the game.
     */
    FONT_FACE_RAINY_HEARTS,

    /**
     * Font face for decorative text pieces.
     */
    FONT_FACE_DAYDREAM,

    /**
     * Debug font that can display all Unicode characters.
     */
    FONT_FACE_UNIFONT,
} FontFace;

/**
 * Represents a normal font with various styles.
 *
 * This is used as a key to cache. All fields in this struct is used in the hashing function.
 */
typedef struct
{
    /**
     * The font face to use.
     */
    FontFace face;

    /**
     * The font's size in points.
     */
    float sp;

    /**
     * The font style flags.
     *
     * Font weight does not seem to be properly supported in variable fonts.
     */
    TTF_FontStyleFlags style;
} Font;

/**
 * Represents an options passed in for handling rendering fonts.
 */
typedef struct
{
    /**
     * The hashed font to use.
     *
     * If the font has not been used yet, it will be created on the fly.
     */
    Font font;

    /**
     * The text to render.
     *
     * \warning Sending NULL or leave it dangling is undefined behavior.
     */
    const char *text;

    /**
     * The X co-ord to place the text.
     */
    double x;

    /**
     * The Y co-ord to place the text.
     */
    double y;

    /**
     * The color to display the text in.
     */
    SDL_Color color;

    /**
     * Where to place the text's origin point.
     */
    RenderingOriginType origin;
} FontRenderingOptions;

/**
 * Initializes the engine needed to create fonts.
 *
 * \param app the application to create with
 * \returns true if it was initialized correctly
 */
bool font_engine_init(AppState *app);

/**
 * Renders a text with options.
 *
 * \param opts the rendering options
 */
void font_engine_render_text(FontRenderingOptions opts);

/**
 * Destroys the initialized font engines.
 */
void font_engine_destroy(void);
