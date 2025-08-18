// main.c
//
// The main entrypoint of the program, any events handling, movements, app state
// updating should be delegated to other source files. Do not do actual handling
// in this file.

#define SDL_MAIN_USE_CALLBACKS

#include "app.h"
#include "engine/engine.h"
#include "game/game_scenes.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

static bool SDL_INIT_SUCCESS    = false;
static bool TTF_INIT_SUCCESS    = false;
static bool APP_INIT_SUCCESS    = false;
static bool ENGINE_INIT_SUCCESS = false;

void log_lib_version(void)
{
    int major, minor, patch;
    major = SDL_VERSIONNUM_MAJOR(SDL_VERSION);
    minor = SDL_VERSIONNUM_MINOR(SDL_VERSION);
    patch = SDL_VERSIONNUM_MICRO(SDL_VERSION);
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Using SDL v%d.%d.%d", major, minor, patch);

    major = SDL_VERSIONNUM_MAJOR(SDL_TTF_VERSION);
    minor = SDL_VERSIONNUM_MINOR(SDL_TTF_VERSION);
    patch = SDL_VERSIONNUM_MICRO(SDL_TTF_VERSION);
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Using SDL TTF v%d.%d.%d", major, minor, patch);

    major = SDL_VERSIONNUM_MAJOR(SDL_IMAGE_VERSION);
    minor = SDL_VERSIONNUM_MINOR(SDL_IMAGE_VERSION);
    patch = SDL_VERSIONNUM_MICRO(SDL_IMAGE_VERSION);
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Using SDL Image v%d.%d.%d", major, minor, patch);

    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Using ZLIB v%s", zlibVersion());
}

/**
 * Calls when the program starts.
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    (void)argc;
    (void)argv;

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
    SDL_SetAppMetadata(APPLICATION_NAME, APPLICATION_VERSION, APPLICATION_IDENTIFIER);

    // Initializes components
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL.");
        return SDL_APP_FAILURE;
    }
    SDL_INIT_SUCCESS = true;

    // Echo out versions.
    log_lib_version();

    if (!TTF_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL_ttf.");
        return SDL_APP_FAILURE;
    }
    TTF_INIT_SUCCESS = true;

    AppState *app = app_init();
    *appstate     = app;

    if (app == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize window and renderer.");
        return SDL_APP_FAILURE;
    }
    APP_INIT_SUCCESS = true;

    if (!engine_init(app))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize game engine.");
        return SDL_APP_FAILURE;
    }
    ENGINE_INIT_SUCCESS = true;

    scene_setup();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Bootstrapped application successfully.");
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *state = (AppState *)appstate;

    if (!state->running)
    {
        return SDL_APP_FAILURE;
    }

    engine_iterate(state);
    engine_render(state);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    // Always quit the app after.
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }

    // Parse events if it's not a quit command.
    AppState *app = (AppState *)appstate;
    engine_handle_event(app, event);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppState *app = (AppState *)appstate;

    if (ENGINE_INIT_SUCCESS)
        engine_destroy();

    if (APP_INIT_SUCCESS)
        app_destroy(app);

    if (TTF_INIT_SUCCESS)
        TTF_Quit();

    if (SDL_INIT_SUCCESS)
        SDL_Quit();

    if (result == SDL_APP_SUCCESS)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application finished successfully");
    }
    else
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application failed");
    }
}
