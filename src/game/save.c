#include "game/save.h"

#include "app.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_storage.h"
#include "SDL3/SDL_timer.h"

char *serialize_app_v1(const AppState *app, uint64_t *buflen)
{
    SDL_IOStream *io = SDL_IOFromDynamicMem();
    if (!io)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open an IO for dynamic writing. %s", SDL_GetError());
        return NULL;
    }

    // Write the version
    SDL_WriteU32LE(io, 1);

    // Write the app's settings
    SDL_WriteU32LE(io, app->settings.max_fps);
    SDL_WriteIO(io, &app->settings.scale, sizeof(double));
    SDL_WriteIO(io, &app->settings.show_debug_colliders, sizeof(bool));

    // Okay, now we flush it down to a normal char *.
    int64_t iolen = SDL_GetIOSize(io);
    if (iolen < 0)
    {
        SDL_CloseIO(io);
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to read IO size from dynamic writes. %s", SDL_GetError());
        return NULL;
    }

    char *buf = SDL_malloc(sizeof(char) * (uint64_t)iolen);
    SDL_ReadIO(io, buf, (uint64_t)iolen);
    SDL_CloseIO(io);

    *buflen = (uint64_t)iolen;
    return buf;
}

void deserialize_app_v1(AppState *app, SDL_IOStream *io)
{
    (void)app;
    (void)io;

    SDL_ReadU32LE(io, &app->settings.max_fps);
    SDL_ReadIO(io, &app->settings.scale, sizeof(double));
    SDL_ReadIO(io, &app->settings.show_debug_colliders, sizeof(bool));
}

bool game_save(const AppState *app, const uint8_t slot)
{
    SDL_assert(slot <= 3);
    SDL_assert(app != NULL);

    SDL_Storage *storage = SDL_OpenUserStorage(APPLICATION_ORGANIZATION, APPLICATION_APP_NAME, 0);
    if (!storage)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to open user storage for saving. %s", SDL_GetError());
        return false;
    }

    while (!SDL_StorageReady(storage))
    {
        SDL_Delay(1);
    }

    // We're using v1 of the serializer.
    uint64_t buflen;
    char *buf = serialize_app_v1(app, &buflen);
    if (!buf)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to serialize app.");
        SDL_CloseStorage(storage);
        return false;
    }

    // Check if the directory can be written to.
    SDL_CreateStorageDirectory(storage, "saves");

    char namebuf[20] = {0};
    SDL_snprintf(namebuf, sizeof(namebuf), "saves/%d", slot);
    if (!SDL_WriteStorageFile(storage, namebuf, buf, buflen))
    {
        SDL_CloseStorage(storage);
        SDL_free(buf);
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to write into storage to save.");
        return false;
    }

    SDL_CloseStorage(storage);
    SDL_free(buf);
    return true;
}

bool game_load(AppState *app, const uint8_t slot)
{
    SDL_assert(app != NULL && slot <= 3);

    SDL_Storage *storage = SDL_OpenUserStorage(APPLICATION_ORGANIZATION, APPLICATION_APP_NAME, 0);
    if (!storage)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to open user storage for saving. %s", SDL_GetError());
        return false;
    }

    while (!SDL_StorageReady(storage))
    {
        SDL_Delay(1);
    }

    char namebuf[20] = {0};
    SDL_snprintf(namebuf, sizeof(namebuf), "saves/%d", slot);

    SDL_PathInfo info;
    if (!SDL_GetStoragePathInfo(storage, namebuf, &info))
    {
        // File doesn't exist.
        SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "Can't load a save file at slot %d because it doesn't exist.", slot);
        SDL_CloseStorage(storage);
        return false;
    }

    char *filebuf = SDL_malloc(sizeof(char) * info.size);
    if (!filebuf)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory to read a save file at slot %d.", slot);
        SDL_CloseStorage(storage);
        return false;
    }

    if (!SDL_ReadStorageFile(storage, namebuf, filebuf, info.size))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to read save file at %s.", namebuf);
        SDL_free(filebuf);
        SDL_CloseStorage(storage);
        return false;
    }

    // We don't need the storage anymore.
    SDL_CloseStorage(storage);

    // Open an IOStream for the thing.
    SDL_IOStream *io = SDL_IOFromConstMem(filebuf, info.size);
    if (!io)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open an IOStream to read save file.");
        SDL_free(filebuf);
        return false;
    }

    uint32_t version;
    SDL_ReadU32LE(io, &version);
    switch (version)
    {
    case 1:
        deserialize_app_v1(app, io);
        break;
    }

    SDL_free(filebuf);
    SDL_CloseIO(io);
    return true;
}
