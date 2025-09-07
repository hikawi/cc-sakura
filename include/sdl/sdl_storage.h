/**
 * \file sdl/sdl_storage.h
 *
 * Wrapper for operations on filesystems (including a read-only file system and a writable file system for games,
 * interfaced by SDL)
 */

#pragma once

#include <cstddef>
#include <expected>
#include <memory>
#include <optional>
#include <SDL3/SDL_storage.h>
#include <string>
#include <vector>

namespace sdl
{

/**
 * Virtual interface for all storage types.
 */
class istorage
{
  public:
    virtual ~istorage() = default;

    /**
     * Checks if the storage is ready or not.
     *
     * \returns true if the storage is ready
     */
    virtual bool is_ready() const noexcept = 0;

    /**
     * Blocks execution until the storage is ready to operate.
     */
    virtual void wait_until_ready() const noexcept = 0;

    /**
     * Retrieves the size of a file in the storage.
     *
     * \param path the path to query
     * \returns an expected result for the file size if exists
     */
    virtual std::expected<uint64_t, std::string> get_file_size(const std::string path) const noexcept = 0;

    /**
     * Reads all of the contents of a file in the storage.
     *
     * \param path the path to read from
     * \returns a byte array optional of what was read
     */
    virtual std::optional<std::vector<std::byte>> read_file(const std::string path) const noexcept = 0;
};

/**
 * Concrete implementation of storage.
 */
class storage : public istorage
{
  public:
    /**
     * Wraps a SDL storage with a concrete implementation.
     *
     * \param storage the storage to wrap
     */
    storage(std::unique_ptr<SDL_Storage, bool (*)(SDL_Storage *)> storage);
    bool is_ready() const noexcept override;
    void wait_until_ready() const noexcept override;
    std::expected<uint64_t, std::string> get_file_size(const std::string path) const noexcept override;
    std::optional<std::vector<std::byte>> read_file(const std::string path) const noexcept override;

  private:
    std::unique_ptr<SDL_Storage, bool (*)(SDL_Storage *)> m_storage;
};

/**
 * Opens the title storage of the application.
 *
 * \returns a storage implementation
 */
std::unique_ptr<istorage> open_title_storage();

} // namespace sdl
