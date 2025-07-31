/**
 * \file game/save.h
 *
 * Serialization and de-serialization for the game's save file.
 */

#pragma once

#include "app.h"

/**
 * Saves the application's settings state.
 *
 * \param settings the settings to save
 * \returns true if the game settings saved correctly
 */
bool game_settings_save(const Settings settings);

/**
 * Loads the application's settings from the storage.
 *
 * \param settings the settings to save into
 * \returns true if it was loaded correctly
 */
bool game_settings_load(Settings *settings);
