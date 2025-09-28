#include "engine/sprite.h"

#include "sdl/sdl_error.h"
#include "sdl/sdl_image.h"
#include "sdl/sdl_iostream.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_rect.h"
#include "sdl/sdl_render.h"

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace ccsakura
{

sprite_cache::sprite_cache()
{
    sdl::log_trace("ccsakura::sprite_cache constructed");
}

isprite &sprite_cache::operator[](std::string name)
{
    return *m_spr_cache[name];
}

bool sprite_cache::has(const std::string name) const noexcept
{
    return m_spr_cache.find(name) != m_spr_cache.end();
}

void sprite_cache::insert(const std::string key, std::unique_ptr<isprite> value) noexcept
{
    m_spr_cache.insert_or_assign(key, std::move(value));
}

void sprite_cache::clear() noexcept
{
    m_spr_cache.clear();
}

sprite_cache::~sprite_cache()
{
    sdl::log_trace("ccsakura::sprite_cache destroyed");
}

sprite::sprite(std::string name, sdl::iiostream &io, sdl::irenderer &renderer) : m_name(name)
{
    setup(io, renderer);
    sdl::log_trace("ccsakura::sprite constructed with {}", name);
}

void sprite::setup(sdl::iiostream &io, sdl::irenderer &renderer)
{
    if (io.status() != sdl::iostatus::ready)
    {
        sdl::log_error("Sprite setup called with iostream status: {}, expecting {} (ready)",
                       static_cast<int>(io.status()), static_cast<int>(sdl::iostatus::ready));
        throw std::invalid_argument("Passed in IOStream is not ready to read");
    }

    uint32_t spr_version = 0;
    io.read_u32_le(spr_version);

    sdl::log_debug("Sprite {} is being setup with version {}", m_name, spr_version);
    switch (spr_version)
    {
    case 1:
        setup_v1(io, renderer);
        break;
    default:
        sdl::log_critical("Sprite file {} has unsupported version {}", m_name, spr_version);
        throw std::runtime_error("Sprite " + m_name + " has unsupported version");
    }
}

void sprite::setup_v1(sdl::iiostream &io, sdl::irenderer &renderer)
{
    uint64_t img_size = 0;
    io.read_u64_le(img_size);
    sdl::log_debug("Sprite {} load: image is {} bytes", m_name, img_size);

    std::vector<std::byte> img_buf(img_size);
    size_t total_read = 0;
    while (total_read < img_size)
    {
        size_t buf_read = io.read(img_buf.data() + total_read, 4096, static_cast<size_t>(img_size) - total_read);
        sdl::log_trace("Sprite {} load: want to read {}, actually read {}", m_name, 4096, buf_read);

        // Premature exit
        if (buf_read == 0)
        {
            if (io.status() == sdl::iostatus::eof)
            {
                sdl::log_error("Unexpected EOF from reading sprite {}", m_name);
                throw std::runtime_error("Unexpected EOF from reading sprite " + m_name);
            }
            else
            {
                sdl::log_error("EOF when there's still more to read: {}", sdl::get_error());
                throw std::runtime_error("EOF when there's still more to read");
            }
        }

        total_read += buf_read;
    }

    // Create a new IOStream for SDL Image to read into
    sdl::iostream img_io(img_buf);
    std::unique_ptr<sdl::isurface> img_surface = sdl::image::load(img_io);

    // Create a GPU-accelerated texture from the surface
    std::unique_ptr<sdl::itexture> img_texture = renderer.create_texture(*img_surface);
    img_texture->set_scale_mode(sdl::scale_mode::nearest);
    io.read_u32_le(m_width);
    io.read_u32_le(m_height);
    sdl::log_debug("Sprite {} load: taken at {}x{}", m_name, m_width, m_height);

    // Start reading frame tags.
    uint32_t frame_tags_count = 0;
    io.read_u32_le(frame_tags_count);
    m_tags.reserve(frame_tags_count);
    sdl::log_debug("Sprite {} load: reserving for {} frame tags", m_name, frame_tags_count);

    // Read each individual frame tag.
    for (uint32_t i = 0; i < frame_tags_count; i++)
    {
        sprite_frame_tag tag;

        // Read name
        uint32_t name_len = 0;
        io.read_u32_le(name_len);
        tag.name = std::string(name_len, '\0');
        io.read(tag.name.data(), name_len, name_len);

        // Read two other stuff
        io.read_u32_le(tag.from);
        io.read_u32_le(tag.to);

        // Assign into the map.
        m_tags[tag.name] = tag;
    }

    // Start reading frames
    uint32_t frames_count = 0;
    io.read_u32_le(frames_count);
    m_frames.reserve(frames_count);
    sdl::log_debug("Sprite {} load: reserving for {} frames", m_name, frames_count);

    for (uint32_t i = 0; i < frames_count; i++)
    {
        sprite_frame &frame = m_frames[i];
        io.read_u32_le(frame.source_w);
        io.read_u32_le(frame.source_h);

        uint32_t x, y, w, h;
        io.read_u32_le(x);
        io.read_u32_le(y);
        io.read_u32_le(w);
        io.read_u32_le(h);
        frame.spr_source_size = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};

        io.read_u32_le(x);
        io.read_u32_le(y);
        io.read_u32_le(w);
        io.read_u32_le(h);
        frame.frame = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};

        io.read_u32_le(frame.duration);
    }

    m_texture = std::move(img_texture);
    sdl::log_info("Successfully loaded sprite {}", m_name);
}

void sprite::render(const sdl::irenderer &renderer, const sdl::fpoint pos, const render_origin origin) const noexcept
{
    sdl::texture_render_options opts(*m_texture.get());
    opts.dst(pos).render_origin(origin);
    renderer.render_texture(opts);
}

sprite::~sprite()
{
    sdl::log_trace("ccsakura::sprite {} destroyed", m_name);
}

isprite_cache *sprite::s_cache = nullptr;
std::function<std::unique_ptr<sdl::istorage>()> sprite::s_open_storage = {};
sdl::irenderer *sprite::s_renderer = nullptr;

isprite &sprite::named(const std::string name)
{
    if (!s_cache)
    {
        sdl::log_critical("Sprite class does not have sprite cache set");
        throw std::runtime_error("Sprite class does not have sprite cache set");
    }
    if (!s_open_storage)
    {
        sdl::log_critical("Sprite class does not have a way to open the title storage");
        throw std::runtime_error("Sprite class does not have a way to open the title storage");
    }
    if (!s_renderer)
    {
        sdl::log_critical("Sprite class does not have a specified renderer");
        throw std::runtime_error("Sprite class does not have a specified renderer");
    }

    if (s_cache->has(name))
    {
        return (*s_cache)[name];
    }

    std::unique_ptr<sdl::istorage> storage = s_open_storage();
    storage->wait_until_ready();
    std::optional<std::vector<std::byte>> file = storage->read_file("assets/spr/" + name + ".sprite");
    if (!file)
    {
        sdl::log_critical("Sprite {} can not be read", name);
        throw std::runtime_error("Failed to read sprite file");
    }

    sdl::iostream io(file.value());
    std::unique_ptr<isprite> spr = std::make_unique<sprite>(name, io, *s_renderer);
    s_cache->insert(name, std::move(spr));
    return (*s_cache)[name];
}

void sprite::use_cache(isprite_cache &cache)
{
    s_cache = &cache;
}

void sprite::use_storage_opener(std::function<std::unique_ptr<sdl::istorage>()> func)
{
    s_open_storage = func;
}

void sprite::use_renderer(sdl::irenderer &renderer)
{
    s_renderer = &renderer;
}

} // namespace ccsakura
