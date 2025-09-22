/**
 * \file sdl/sdl_iostream.h
 *
 * Wrapper for IOStreams similar to standard library's iostream with automatic closure for out-of-scope. I prefer to use
 * this module as SDL provides helpers for dealing with endianness, and it's more precise than standard library's
 * ofstream.
 */

#pragma once

#include <cstddef>
#include <memory>
#include <SDL3/SDL_iostream.h>
#include <vector>

namespace sdl
{

/**
 * Represents the status of an IOStream.
 */
enum class iostatus
{
    eof = SDL_IO_STATUS_EOF,             ///< represents the end-of-file signal of an iostream
    error = SDL_IO_STATUS_ERROR,         ///< the iostream has an error
    ready = SDL_IO_STATUS_READY,         ///< the iostream is ready to read or write to
    not_ready = SDL_IO_STATUS_NOT_READY, ///< the iostream has an error or eof, can't read or write
    readonly = SDL_IO_STATUS_READONLY,   ///< the stream is read-only
    writeonly = SDL_IO_STATUS_WRITEONLY, ///< the stream is write-only
};

/**
 * Virtual interface for an SDL IOStream.
 */
class iiostream
{
  public:
    virtual ~iiostream() = default;

    /**
     * Retrieves the IOStream wrapped by this wrapper.
     *
     * \returns the wrapped pointer
     */
    virtual SDL_IOStream *get() const noexcept = 0;

    /**
     * Queries for the status of an IOStream.
     *
     * \returns the iostream status
     */
    virtual iostatus status() const noexcept = 0;

    /**
     * Reads an unsigned 32-bit lower-endian number.
     *
     * \param out the output value to write to
     * \warning this can throw an exception
     */
    virtual void read_u32_le(uint32_t &out) = 0;

    /**
     * Reads an unsigned 64-bit lower-endian number.
     *
     * \param out the output value to put into
     * \warning this can throw an exception
     */
    virtual void read_u64_le(uint64_t &out) = 0;

    /**
     * Reads a number of bytes into the buffer, the buffer must be writable.
     *
     * \param buf the buffer to write to
     * \param len the length of bytes to read
     * \returns the number of bytes read
     */
    virtual size_t read(void *buf, size_t len) = 0;
};

/**
 * Wraps an SDL's IOStream.
 */
class iostream : public iiostream
{
  public:
    /**
     * Wraps the provided memory as if it's an IOStream.
     *
     * \param span the span for a byte array to view into
     */
    iostream(std::vector<std::byte> span);

    /**
     * Creates a new empty IOStream to dynamic memory.
     */
    iostream();
    ~iostream() override;

    iostatus status() const noexcept override;
    void read_u32_le(uint32_t &out) override;
    void read_u64_le(uint64_t &out) override;
    size_t read(void *buf, size_t len) override;

    SDL_IOStream *get() const noexcept override;

  private:
    std::vector<std::byte> m_span;
    std::unique_ptr<SDL_IOStream, bool (*)(SDL_IOStream *)> m_iostream;
};

} // namespace sdl
