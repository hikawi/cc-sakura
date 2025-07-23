/**
 * \file engine/logger.h
 *
 * The engine's file logging module to allow logging into a file for debugging purposes.
 */

#include "app.h"

#include <stdbool.h>

/**
 * Initializes the logger module.
 *
 * \param app the main app state
 * \returns true if the module was enabled correctly
 */
bool logger_init(AppState *app);

/**
 * Destroys the logger module.
 *
 * \param app the main app state
 */
void logger_destroy(AppState *app);
