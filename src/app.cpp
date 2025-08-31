#include "app.h"

#include "sdl/sdl_log.h"

namespace ccsakura
{

app::app()
{
    sdl::log_trace("ccsakura::app constructed.");
}

app::~app()
{
    sdl::log_trace("ccsakura::app destructed.");
}

} // namespace ccsakura
