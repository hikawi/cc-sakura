#include "engine/component/text.h"

namespace ccsakura::components
{

void text::set(std::string_view str) noexcept
{
    value = str;
}

} // namespace ccsakura::components
