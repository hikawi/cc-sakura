#include "misc/hashmap.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_stdinc.h"

static inline Uint32 hash_uint32(Uint32 key)
{
    return (key * 2654435761u) % HASH_MAP_MAX_BUCKETS;
}

HashMap *hash_map_init(void)
{
    HashMap *map = SDL_malloc(sizeof(HashMap));
    SDL_memset(map, 0, sizeof(HashMap));
    return map;
}

bool hash_map_has_key(HashMap *map, Uint32 key)
{
    return hash_map_get(map, key) != NULL;
}

bool hash_map_has_value(HashMap *map, void *value)
{
    for (int i = 0; i < HASH_MAP_MAX_BUCKETS; ++i)
    {
        HashMapNode *node = map->nodes[i];
        while (node)
        {
            if (node->value == value)
            {
                return true;
            }
            node = node->next;
        }
    }
    return false;
}

void *hash_map_put(HashMap *map, Uint32 key, void *value)
{
    Uint32 idx = hash_uint32(key);
    HashMapNode *prev = NULL;
    HashMapNode *cur = map->nodes[idx];

    while (cur)
    {
        // Replacement.
        if (cur->key == key)
        {
            void *ret = cur->value;
            cur->value = value;
            return ret;
        }

        prev = cur;
        cur = cur->next;
    }

    // That key doesn't exist yet.
    HashMapNode *node = SDL_malloc(sizeof(HashMapNode));
    node->key = key;
    node->value = value;
    node->next = NULL;
    if (prev)
    {
        // The chain is already started.
        prev->next = node;
    }
    else
    {
        // The chain doesn't exist yet.
        map->nodes[idx] = node;
    }

    map->size++;
    return NULL;
}

void *hash_map_remove(HashMap *map, Uint32 key)
{
    Uint32 idx = hash_uint32(key);
    HashMapNode *prev = NULL;
    HashMapNode *cur = map->nodes[idx];

    while (cur)
    {
        // Remove existing node.
        if (cur->key == key)
        {
            // Depends on whether it's a start of a chain or a inner node.
            if (prev)
            {
                prev->next = cur->next;
            }
            else
            {
                map->nodes[idx] = cur->next;
            }

            void *ret = cur->value;
            SDL_free(cur);
            map->size--;
            return ret;
        }

        prev = cur;
        cur = cur->next;
    }

    return NULL;
}

void *hash_map_get(HashMap *map, Uint32 key)
{
    HashMapNode *cur = map->nodes[hash_uint32(key)];
    while (cur)
    {
        if (cur->key == key)
            return cur->value;
        cur = cur->next;
    }
    return NULL;
}

void hash_map_iterate(HashMap *map, Uint32 *keys, void **values)
{
    SDL_assert(keys != NULL && values != NULL);

    Uint32 idx = 0;
    for (int i = 0; i < HASH_MAP_MAX_BUCKETS; i++)
    {
        HashMapNode *node = map->nodes[idx];
        while (node)
        {
            keys[idx] = node->key;
            values[idx] = node->value;
            idx++;
            node = node->next;
        }
    }

    SDL_assert(idx == map->size);
}

void map_node_destroy(HashMapNode *node)
{
    while (node)
    {
        HashMapNode *next = node->next;
        SDL_free(node);
        node = next;
    }
}

void hash_map_clear(HashMap *map)
{
    for (int i = 0; i < HASH_MAP_MAX_BUCKETS; i++)
    {
        map_node_destroy(map->nodes[i]);
        map->nodes[i] = NULL;
    }

    map->size = 0;
}

void hash_map_destroy(HashMap *map)
{
    if (!map)
        return;

    hash_map_clear(map);
    SDL_free(map);
}
