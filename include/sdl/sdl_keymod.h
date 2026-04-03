#pragma once

#include <cstdint>
#include <SDL3/SDL_keycode.h>

namespace sdl
{

enum class keymod : uint16_t
{
    none = SDL_KMOD_NONE,
    lshift = SDL_KMOD_LSHIFT,
    rshift = SDL_KMOD_RSHIFT,
    level5 = SDL_KMOD_LEVEL5,
    lctrl = SDL_KMOD_LCTRL,
    rctrl = SDL_KMOD_RCTRL,
    lalt = SDL_KMOD_LALT,
    ralt = SDL_KMOD_RALT,
    lgui = SDL_KMOD_LGUI,
    rgui = SDL_KMOD_RGUI,
    num = SDL_KMOD_NUM,
    caps = SDL_KMOD_CAPS,
    mode = SDL_KMOD_MODE,
    scroll = SDL_KMOD_SCROLL,
    ctrl = SDL_KMOD_CTRL,
    shift = SDL_KMOD_SHIFT,
    alt = SDL_KMOD_ALT,
    gui = SDL_KMOD_GUI
};

} // namespace sdl

