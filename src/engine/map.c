#include "engine/map.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "engine/sprite.h"

int map_compute_index(Map *map, Uint32 x, Uint32 y)
{
    if (x >= map->w || y >= map->h)
        return -1;
    return (int)(y * map->w + x);
}

/**
 * Rescans the map tiles and checks for neighbors.
 */
void map_autotile(Map *map)
{
    for (Uint32 y = 0; y < map->h; y++)
    {
        for (Uint32 x = 0; x < map->w; x++)
        {
            int idx = map_compute_index(map, x, y);
            int north = map_compute_index(map, x, y - 1);
            int south = map_compute_index(map, x, y + 1);
            int east = map_compute_index(map, x + 1, y);
            int west = map_compute_index(map, x - 1, y);

            MapTile val = map->tiles[idx].tile;
            map->tiles[idx].dir = 0;

            if (north >= 0 && map->tiles[north].tile == val)
            {
                map->tiles[idx].dir |= NODE_DIR_N;
            }
            if (south >= 0 && map->tiles[south].tile == val)
            {
                map->tiles[idx].dir |= NODE_DIR_S;
            }
            if (west >= 0 && map->tiles[west].tile == val)
            {
                map->tiles[idx].dir |= NODE_DIR_W;
            }
            if (east >= 0 && map->tiles[east].tile == val)
            {
                map->tiles[idx].dir |= NODE_DIR_E;
            }
        }
    }
}

/**
 * Initializes the map from the version 1 of the file.
 */
Map *map_init_v1(SDL_IOStream *io)
{
    Map *map = SDL_malloc(sizeof(Map));

    // Read the map's name.
    Uint32 name_len;
    SDL_ReadU32LE(io, &name_len);
    char *buf = SDL_calloc(name_len + 1, sizeof(char));
    SDL_ReadIO(io, buf, name_len);
    map->name = buf;

    // Read the max size of the map coords.
    SDL_ReadU32LE(io, &map->w);
    SDL_ReadU32LE(io, &map->h);

    // Construct the map's matrix of nodes.
    map->tiles = SDL_malloc(map->w * map->h * sizeof(MapNode));
    for (Uint32 i = 0; i < map->w * map->h; i++)
    {
        map->tiles[i].dir = 0;
        map->tiles[i].tile = TILE_AIR;
    }

    // Read the coord list.
    Uint32 coords_len;
    SDL_ReadU32LE(io, &coords_len);
    for (Uint32 i = 0; i < coords_len; i++)
    {
        Uint32 x, y, v;
        SDL_ReadU32LE(io, &x);
        SDL_ReadU32LE(io, &y);
        SDL_ReadU32LE(io, &v);

        map->tiles[y * map->w + x].tile = (MapTile)v;
    }

    map_autotile(map);
    return map;
}

Map *map_init(const char *name)
{
    SDL_Log("Attempting to load map %s", name);

    char buf[256] = "assets/map/";
    SDL_strlcat(buf, name, sizeof(buf));
    SDL_strlcat(buf, ".map", sizeof(buf));

    SDL_IOStream *io = SDL_IOFromFile(buf, "rb");
    if (io == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Can't find map file of name %s", name);
        return NULL;
    }

    Map *map = NULL;
    Uint32 version;
    SDL_ReadU32LE(io, &version);

    switch (version)
    {
    case 1:
        map = map_init_v1(io);
        break;
    default:
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Unknown version of map file. Can't load %s", name);
        break;
    }

    SDL_CloseIO(io);
    return map;
}

void map_tile_sprite(Sprite *spr, MapTile tile)
{
    switch (tile)
    {
    case TILE_AIR:
        sprite_set_animation(spr, NULL);
        break;
    case TILE_WOOD:
        sprite_set_animation(spr, "wood");
        break;
    }
}

void map_destroy(Map *map)
{
    if (!map)
        return;

    SDL_free(map->name);
    SDL_free(map->tiles);
    SDL_free(map);
}
