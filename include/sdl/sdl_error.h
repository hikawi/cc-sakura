/**
 * \file sdl/sdl_error.h
 *
 * Provides an error message and handling SDL error messages.
 */

#pragma once

#include <string>

namespace sdl
{

/**
 * Retrieves the last error from SDL.
 *
 * \returns the last error that happened
 */
std::string get_error();

} // namespace sdl
