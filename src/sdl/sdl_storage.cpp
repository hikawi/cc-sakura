#include "sdl/sdl_storage.h"

#include "sdl/sdl_log.h"
#include "sdl/sdl_timer.h"

#include <memory>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_storage.h>

namespace sdl
{

using namespace std::chrono_literals;

storage::storage(std::unique_ptr<SDL_Storage, bool (*)(SDL_Storage *)> storage) : m_storage(std::move(storage))
{
}

bool storage::is_ready() const noexcept
{
    return SDL_StorageReady(m_storage.get());
}

void storage::wait_until_ready() const noexcept
{
    while (!is_ready())
    {
        sdl::delay(1ms);
    }
}

std::expected<uint64_t, std::string> storage::get_file_size(const std::string path) const noexcept
{
    uint64_t size = 0;
    if (!SDL_GetStorageFileSize(m_storage.get(), path.c_str(), &size))
    {
        sdl::log_error("Unable to read file size at path {}", path);
        return std::unexpected(SDL_GetError());
    }

    return size;
}

std::optional<std::vector<std::byte>> storage::read_file(const std::string path) const noexcept
{
    const auto file_size = get_file_size(path);
    if (!file_size)
    {
        return std::nullopt;
    }

    std::vector<std::byte> file_buf(file_size.value());
    if (!SDL_ReadStorageFile(m_storage.get(), path.c_str(), file_buf.data(), file_size.value()))
    {
        return std::nullopt;
    }

    return file_buf;
}

std::unique_ptr<istorage> open_title_storage()
{
    auto ptr = std::unique_ptr<SDL_Storage, bool (*)(SDL_Storage *)>(SDL_OpenTitleStorage(NULL, 0), SDL_CloseStorage);
    if (!ptr)
    {
        sdl::log_critical("Unable to open the title storage: {}", SDL_GetError());
        throw std::runtime_error("Unable to open the title storage");
    }
    return std::make_unique<storage>(std::move(ptr));
}

} // namespace sdl
