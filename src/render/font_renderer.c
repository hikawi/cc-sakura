#include "render/font_renderer.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "render/renderer.h"
#include <string.h>

static const char *PIXELIFY_SANS_PATH = "assets/font/pixelify-sans.ttf";
static const char *UNIFONT_PATH = "assets/font/unifont.ttf";

static TTF_Font *pixelify_font = NULL;
static TTF_Font *unifont_font = NULL;

bool init_font_faces(void)
{
  pixelify_font = TTF_OpenFont(PIXELIFY_SANS_PATH, 32);
  if (pixelify_font == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Couldn't load Pixelify Sans font. Are you in the correct path?");
    return false;
  }

  unifont_font = TTF_OpenFont(UNIFONT_PATH, 32);
  if (unifont_font == NULL)
  {
    TTF_CloseFont(pixelify_font);
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "COuldn't load Unifont font. Are you in the correct path?");
    return false;
  }

  return true;
}

void draw_font(FontFace face, const char *str, SDL_Color color,
               double ori_x, double ori_y, FontOrigin ori_type)
{
  TTF_Font *font;
  switch (face)
  {
  case FONT_FACE_PIXELIFY_SANS:
    font = pixelify_font;
    break;
  default:
    font = unifont_font;
    break;
  }

  SDL_Renderer *renderer = get_current_renderer();
  SDL_Surface *surface = TTF_RenderText_Blended(font, str, strlen(str), color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);

  // Calculate where to put on the screen.
  SDL_FRect dstrect;
  dstrect.x = (float)ori_x;
  dstrect.y = (float)ori_y;
  SDL_GetTextureSize(texture, &dstrect.w, &dstrect.h);

  switch (ori_type)
  {
  case FONT_ORIGIN_TOP_LEFT: // Do nothing.
    break;
  case FONT_ORIGIN_TOP_CENTER: // Move origin X to left by half.
    dstrect.x -= dstrect.w / 2;
    break;
  case FONT_ORIGIN_TOP_RIGHT: // Move origin X to left full.
    dstrect.x -= dstrect.w;
    break;
  case FONT_ORIGIN_MIDDLE_LEFT: // Move origin Y down by half.
    dstrect.y -= dstrect.h / 2;
    break;
  case FONT_ORIGIN_MIDDLE_CENTER: // Move both origins by half.
    dstrect.x -= dstrect.w / 2;
    dstrect.y -= dstrect.h / 2;
    break;
  case FONT_ORIGIN_MIDDLE_RIGHT: // Move X by full and Y by half.
    dstrect.x -= dstrect.w;
    dstrect.y -= dstrect.h / 2;
    break;
  case FONT_ORIGIN_BOTTOM_LEFT: // Move Y by full.
    dstrect.y -= dstrect.h;
    break;
  case FONT_ORIGIN_BOTTOM_CENTER: // Move Y full and X half.
    dstrect.x -= dstrect.w / 2;
    dstrect.y -= dstrect.h;
    break;
  case FONT_ORIGIN_BOTTOM_RIGHT: // Move Y full and X full.
    dstrect.x -= dstrect.w;
    dstrect.y -= dstrect.h;
    break;
  }

  // Ok now we can render.
  SDL_RenderTexture(renderer, texture, NULL, &dstrect);

  // Clean up after use.
  SDL_DestroySurface(surface);
  SDL_DestroyTexture(texture);
}

void destroy_font_faces(void)
{
  TTF_CloseFont(pixelify_font);
  TTF_CloseFont(unifont_font);
}
