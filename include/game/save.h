/**
 * \file game/save.h
 *
 * Serialization and de-serialization for the game's save file.
 */

#pragma once

#include "app.h"

/**
 * Saves the application.
 *
 * \param app the app to save
 * \param slot the slot to save to
 * \warning This uses a mutex to lock access.
 */
bool game_save(const AppState *app, const uint8_t slot);

/**
 * Loads the application's save file.
 *
 * \param app the app to load into
 * \param slot the slot to pick from
 * \returns true if the load was successful, false if the slot did not succeed
 */
bool game_load(AppState *app, const uint8_t slot);
