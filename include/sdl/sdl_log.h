/**
 * \file sdl/sdl_log.h
 *
 * Template function wrappers for SDL's logging module, but using fmt-style formatting.
 */

#pragma once

#include <format>
#include <SDL3/SDL_log.h>
#include <string_view>

namespace sdl
{

/**
 * Clears all log messages' prefix at all priorities.
 */
void clear_log_priority_prefix();

/**
 * Logs a message at the TRACE level.
 *
 * These messages should be included in very deep and granular object lifecycles to track.
 *
 * \param fmt the formatter
 * \param args the args to format
 */
template <typename... Args> void log_trace(std::string_view fmt, Args &&...args)
{
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, "\033[94mTRACE > %s\033[0m", msg.c_str());
}

/**
 * Logs a message at the DEBUG level.
 *
 * These messages should indicate what actions are taking place, track the data flow of the app.
 *
 * \param fmt the formatter
 * \param args the args to format
 */
template <typename... Args> void log_debug(std::string_view fmt, Args &&...args)
{
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "\033[92mDEBUG > %s\033[0m", msg.c_str());
}

/**
 * Logs a message at the INFO level.
 *
 * Use this for major milestones in the application. This includes the application successfully starting up, a
 * significant module being initialized, a database connection being established, or a main loop starting. These are the
 * logs you want to see in production to know the application is running as expected
 *
 * \param fmt the formatter
 * \param args the args to format
 */
template <typename... Args> void log_info(std::string_view fmt, Args &&...args)
{
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "\033[96mINFO  > %s\033[0m", msg.c_str());
}

/**
 * Logs a message at the WARN level.
 *
 * Log when a function receives unexpected or invalid data but can recover, when a file is not found but an optional
 * fallback exists, or when performance is degraded. It's an alert that something needs to be looked at, but not
 * immediately.
 *
 * \param fmt the formatter
 * \param args the args to format
 */
template <typename... Args> void log_warn(std::string_view fmt, Args &&...args)
{
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "\033[93mWARN  > %s\033[0m", msg.c_str());
}

/**
 * Logs a message at the ERROR level.
 *
 * Use this when a resource fails to load (e.g., a critical texture or sound file), a network connection is lost, or a
 * required function returns an error code that cannot be handled.
 *
 * \param fmt the formatter
 * \param args the args to format
 */
template <typename... Args> void log_error(std::string_view fmt, Args &&...args)
{
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "\033[91mERROR > %s\033[0m", msg.c_str());
}

/**
 * Logs a message at the CRITICAL level.
 *
 * Log when a memory allocation fails, an unrecoverable crash is about to happen, or a critical dependency is missing.
 * This type of log should be treated as an immediate signal that the application is about to stop functioning.
 *
 * \param fmt the formatter
 * \param args the args to format
 */
template <typename... Args> void log_critical(std::string_view fmt, Args &&...args)
{
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "\033[95mCRIT  > %s\033[0m", msg.c_str());
}

} // namespace sdl
