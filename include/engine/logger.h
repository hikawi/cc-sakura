// engine/logger.h
//
// Represents the application's logging mechanism.

#include "app.h"
#include <stdbool.h>

/**
 * Initializes the logger module.
 */
bool logger_init(AppState *app);

/**
 * Destroys the logger module.
 */
void logger_destroy(AppState *app);
