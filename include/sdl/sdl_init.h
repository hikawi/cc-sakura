/**
 * \file sdl/sdl_init.h
 *
 * This module is in charge of initializing and de-initializing SDL and related modules for the app to run.
 */

#pragma once

namespace sdl
{

/**
 * Initializes all related SDL's components (video, audio, font-files).
 *
 * \returns true if the initialization succeeded.
 */
bool init();

/**
 * Tears down all SDL's components.
 */
void quit() noexcept;

} // namespace sdl
