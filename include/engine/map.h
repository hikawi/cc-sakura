// engine/map.h
//
// Represents the map loader from a level file.

#pragma once

#include "SDL3/SDL_stdinc.h"
#include "engine/sprite.h"

#define NODE_DIR_N (1 << 0)
#define NODE_DIR_S (1 << 1)
#define NODE_DIR_W (1 << 2)
#define NODE_DIR_E (1 << 3)

// This isn't used but it's here as a reference for how to tile your sprites.
#define TILE_NEIGHBOR_NONE 0
#define TILE_NEIGHBOR_N (NODE_DIR_N)
#define TILE_NEIGHBOR_S (NODE_DIR_S)
#define TILE_NEIGHBOR_W (NODE_DIR_W)
#define TILE_NEIGHBOR_E (NODE_DIR_E)
#define TILE_NEIGHBOR_NS (NODE_DIR_N | NODE_DIR_S)
#define TILE_NEIGHBOR_NW (NODE_DIR_N | NODE_DIR_W)
#define TILE_NEIGHBOR_NE (NODE_DIR_N | NODE_DIR_E)
#define TILE_NEIGHBOR_SW (NODE_DIR_S | NODE_DIR_W)
#define TILE_NEIGHBOR_SE (NODE_DIR_S | NODE_DIR_E)
#define TILE_NEIGHBOR_WE (NODE_DIR_W | NODE_DIR_E)
#define TILE_NEIGHBOR_NSW (NODE_DIR_N | NODE_DIR_S | NODE_DIR_W)
#define TILE_NEIGHBOR_NSE (NODE_DIR_N | NODE_DIR_S | NODE_DIR_E)
#define TILE_NEIGHBOR_NWE (NODE_DIR_N | NODE_DIR_W | NODE_DIR_E)
#define TILE_NEIGHBOR_SWE (NODE_DIR_S | NODE_DIR_W | NODE_DIR_E)
#define TILE_NEIGHBOR_ALL (NODE_DIR_N | NODE_DIR_S | NODE_DIR_W | NODE_DIR_E)

/**
 * Represents the tile type of a map.
 */
typedef enum
{
    TILE_AIR = 0,
    TILE_WOOD,
} MapTile;

/**
 * Represents a node of the map.
 */
typedef struct
{
    MapTile tile;
    int dir; // The direction bit mask.
} MapNode;

/**
 * Represents a level's map.
 */
typedef struct
{
    char *name;
    Uint32 w;
    Uint32 h;
    MapNode *tiles;
} Map;

/**
 * Initializes a map from a provided .map file.
 */
Map *init_map_from_file(const char *name);

/**
 * Sets the tile used for the map.
 */
void set_map_tile_sprite(Sprite *spr, MapTile tile);

/**
 * Destroys all memory used by the map.
 */
void destroy_map(Map *map);
