#include "sdl/sdl_init.h"

#include "sdl/sdl_log.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_version.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <zlib.h>
#include <zstd.h>

namespace sdl
{

bool init()
{
    // Setup metadata
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, APPLICATION_NAME);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, APPLICATION_IDENTIFIER);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, APPLICATION_VERSION);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");

    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        sdl::log_critical("Unable to initialize SDL components: {}", SDL_GetError());
        return false;
    }
    sdl::log_info("Initialized SDL v{}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);

    if (!TTF_Init())
    {
        sdl::log_critical("Unable to initialize SDL TTF module: {}", SDL_GetError());
        return false;
    }

    sdl::log_info("Initialized SDL TTF v{}.{}.{}", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_TTF_MICRO_VERSION);
    sdl::log_info("Found SDL Image v{}.{}.{}", SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION,
                  SDL_IMAGE_MICRO_VERSION);
    sdl::log_info("Found ZLIB v{}", zlibVersion());
    sdl::log_info("Found ZSTD v{}", ZSTD_VERSION_STRING);

    return true;
}

void quit() noexcept
{
    TTF_Quit();
    SDL_Quit();
}

} // namespace sdl
