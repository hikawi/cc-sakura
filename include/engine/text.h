// engine/text.h
//
// The text rendering part of the engine. This may be used to draw texts
// with various fonts with ease.

#pragma once

#include "SDL3/SDL_pixels.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "engine/renderer.h"
#include <stdbool.h>

/**
 * Represents a font face to be loaded.
 */
typedef enum
{
    // The font face that should be the main font to be used.
    FONT_FACE_RAINY_HEARTS,

    // The font face that should be used as secondary font.
    FONT_FACE_DAYDREAM,

    // The debug font face that can also display all Unicode characters.
    FONT_FACE_UNIFONT,
} FontFace;

/**
 * Represents a normal font with various styles.
 */
typedef struct
{
    FontFace face;
    float sp;
    TTF_FontStyleFlags style;
} Font;

/**
 * Represents an options passed in for handling rendering fonts.
 */
typedef struct
{
    Font font;
    const char *text;
    double x;
    double y;
    SDL_Color color;
    RenderingOriginType origin;
} FontRenderingOptions;

/**
 * Initializes the engine needed to create fonts.
 */
bool init_font_engine(AppState *app);

/**
 * Renders a text.
 */
void render_text(FontRenderingOptions opts);

/**
 * Destroys the initialized font engines.
 */
void destroy_font_engine(void);
