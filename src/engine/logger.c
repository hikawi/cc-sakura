#include "engine/logger.h"

#include "app.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"

static char log_buf[1024] = {0};
static size_t log_buf_len = sizeof(log_buf);

void logger_output_func(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    AppState *app = userdata;
    if (!app || !app->logger.logio)
    {
        return;
    }

    const char *category_str = "";
    const char *priority_str = "";

    switch (category)
    {
    case SDL_LOG_CATEGORY_INPUT:
        category_str = "INP";
        break;
    case SDL_LOG_CATEGORY_RENDER:
        category_str = "REN";
        break;
    case SDL_LOG_CATEGORY_GPU:
        category_str = "GPU";
        break;
    case SDL_LOG_CATEGORY_APPLICATION:
        category_str = "APP";
        break;
    case SDL_LOG_CATEGORY_VIDEO:
        category_str = "VID";
        break;
    case SDL_LOG_CATEGORY_SYSTEM:
        category_str = "SYS";
        break;
    case SDL_LOG_CATEGORY_ERROR:
        category_str = "ERR";
        break;
    case SDL_LOG_CATEGORY_AUDIO:
        category_str = "AUD";
        break;
    case SDL_LOG_CATEGORY_TEST:
        category_str = "TST";
        break;
    case SDL_LOG_CATEGORY_ASSERT:
        category_str = "ASR";
        break;
    default:
        category_str = "???";
        break;
    }

    switch (priority)
    {
    case SDL_LOG_PRIORITY_DEBUG:
        priority_str = "DEBUG";
        break;
    case SDL_LOG_PRIORITY_INFO:
        priority_str = "INFO";
        break;
    case SDL_LOG_PRIORITY_VERBOSE:
        priority_str = "VERBOSE";
        break;
    case SDL_LOG_PRIORITY_COUNT:
        priority_str = "COUNT";
        break;
    case SDL_LOG_PRIORITY_WARN:
        priority_str = "WARN";
        break;
    case SDL_LOG_PRIORITY_CRITICAL:
        priority_str = "CRITICAL";
        break;
    case SDL_LOG_PRIORITY_ERROR:
        priority_str = "ERROR";
        break;
    case SDL_LOG_PRIORITY_TRACE:
        priority_str = "TRACE";
        break;
    case SDL_LOG_PRIORITY_INVALID:
        priority_str = "INVALID";
        break;
    }

    SDL_snprintf(log_buf, log_buf_len, "[%s] | %s > %s\n", priority_str, category_str, message);
    SDL_WriteIO(app->logger.logio, log_buf, SDL_strlen(log_buf));
}

bool logger_init(AppState *app)
{
    if (app->logger.logio)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Logger is already initialized.");
        return true;
    }

    char fbuf[1024] = {0};
    char *pref_path = SDL_GetPrefPath(APPLICATION_ORGANIZATION, APPLICATION_APP_NAME);
    SDL_snprintf(fbuf, sizeof(fbuf), "%slog.txt", pref_path);
    SDL_free(pref_path);
    pref_path = NULL;

    app->logger.logio = SDL_IOFromFile(fbuf, "w");
    if (!app->logger.logio)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to open log file for writing, defaulting to console.");
        return false;
    }

    SDL_SetLogOutputFunction(logger_output_func, app);
    SDL_Log("Opened log file for logging at %s", fbuf);
    return true;
}

void logger_destroy(AppState *app)
{
    if (!app || !app->logger.logio)
    {
        return;
    }

    SDL_CloseIO(app->logger.logio);
    SDL_SetLogOutputFunction(SDL_GetDefaultLogOutputFunction(), NULL);
    app->logger.logio = NULL;
}
