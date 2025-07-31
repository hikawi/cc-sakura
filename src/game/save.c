#include "game/save.h"

#include "app.h"
#include "common.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_storage.h"

void serialize_settings_v1(const Settings settings, SDL_IOStream *io)
{
    SDL_assert(io != NULL);

    SDL_WriteU32LE(io, 1); // version
    SDL_WriteU32LE(io, settings.max_fps);
    SDL_WriteIO(io, &settings.scale, sizeof(double));
    SDL_WriteIO(io, &settings.show_debug_colliders, sizeof(bool));
}

void deserialize_settings_v1(Settings *settings, SDL_IOStream *io)
{
    SDL_assert(io != NULL);

    SDL_ReadU32LE(io, &settings->max_fps);
    SDL_ReadIO(io, &settings->scale, sizeof(double));
    SDL_ReadIO(io, &settings->show_debug_colliders, sizeof(bool));
}

bool game_settings_save(const Settings settings)
{
    bool ret = false;

    SDL_Storage *storage = open_user_storage();
    if (!storage)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open user storage for saving.");
        goto err_free_none;
    }

    // Write into a file.
    SDL_IOStream *io = SDL_IOFromDynamicMem();
    if (!io)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open stream for settings serialization.");
        goto err_free_storage;
    }

    serialize_settings_v1(settings, io);

    // Convert to buffer.
    int64_t buflen = SDL_GetIOSize(io);
    if (buflen < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Can't see stream size for writing settings file");
        goto err_free_io;
    }

    char *buf = SDL_malloc(sizeof(char) * (uint64_t)buflen);
    if (!buf)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory for settings file");
        goto err_free_io;
    }

    SDL_SeekIO(io, 0, SDL_IO_SEEK_SET);
    SDL_ReadIO(io, buf, (uint64_t)buflen);

    if (!SDL_WriteStorageFile(storage, "settings", buf, (uint64_t)buflen))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to write settings file.");
    }
    else
    {
        ret = true;
    }

    SDL_free(buf);
err_free_io:
    SDL_CloseIO(io);
err_free_storage:
    SDL_CloseStorage(storage);
err_free_none:
    return ret;
}

bool game_settings_load(Settings *settings)
{
    const char *filepath = "settings";
    bool ret             = false;

    SDL_Storage *storage = open_user_storage();
    if (!storage)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open user storage for loading settings.");
        goto err_free_none;
    }

    SDL_PathInfo info;
    if (!SDL_GetStoragePathInfo(storage, filepath, &info))
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Couldn't find a settings file to load.");
        goto err_free_storage;
    }

    char *filebuf = SDL_malloc(sizeof(char) * info.size);
    if (!filebuf)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory for settings file");
        goto err_free_storage;
    }

    if (!SDL_ReadStorageFile(storage, filepath, filebuf, info.size))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to read storage file settings. %s", SDL_GetError());
        goto err_free_filebuf;
    }

    SDL_IOStream *io = SDL_IOFromConstMem(filebuf, info.size);
    if (!io)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to open IOStream for settings file. %s", SDL_GetError());
        goto err_free_filebuf;
    }

    uint32_t version = 0;
    SDL_ReadU32LE(io, &version);
    switch (version)
    {
    case 1:
        deserialize_settings_v1(settings, io);
        break;
    }

    ret = true;
    SDL_CloseIO(io);
err_free_filebuf:
    SDL_free(filebuf);
err_free_storage:
    SDL_CloseStorage(storage);
err_free_none:
    return ret;
}
