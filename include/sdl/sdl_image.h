/**
 * \file sdl/sdl_image.h
 *
 * Wrappers for SDL image sister module.
 */

#pragma once

#include "sdl/sdl_iostream.h"
#include "sdl/sdl_surface.h"

#include <memory>

namespace sdl::image
{

/**
 * Reads an arbitrary image from an IOStream and returns it as a surface.
 *
 * \param io the iostream to read from
 * \returns a unique pointer holding a surface
 */
std::unique_ptr<sdl::isurface> load(sdl::iiostream &io);

} // namespace sdl::image
