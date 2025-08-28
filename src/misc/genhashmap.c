#include "misc/genhashmap.h"

#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"

struct GenericHashMapNode
{
    const void *key;
    const void *value;
    GenericHashMapNode *next;
};

GenericHashMap *gen_hash_map_init(void)
{
    GenericHashMap *map = SDL_calloc(1, sizeof(GenericHashMap));

    if (!map)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't allocate memory for a generic hash map");
        return NULL;
    }

    map->capacity = 256;
    map->buckets = SDL_calloc(map->capacity, sizeof(GenericHashMapNode *));

    if (!map->buckets)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't allocate memory for generic hash map buckets");
        SDL_free(map);
        return NULL;
    }

    return map;
}

void gen_hash_map_node_destroy(const GenericHashMap *map, GenericHashMapNode *node)
{
    if (!node)
    {
        return;
    }

    if (map->destroys_key && node->key)
    {
        SDL_free((void *)node->key);
        node->key = NULL;
    }
    if (map->destroys_value && node->value)
    {
        SDL_free((void *)node->value);
        node->value = NULL;
    }
    SDL_free(node);
}

const void *gen_hash_map_put(GenericHashMap *map, const void *key, const void *value)
{
    SDL_assert(map != NULL && key != NULL && value != NULL);
    SDL_assert(map->hash != NULL && map->comparator != NULL);

    uint64_t idx = map->hash(key) % map->capacity;

    GenericHashMapNode *prev = NULL;
    GenericHashMapNode *node = map->buckets[idx];
    while (node)
    {
        if (map->comparator(node->key, key) == 0)
        {
            SDL_Log("f");
            const void *old = node->value;
            node->value = value;

            if (map->destroys_value)
            {
                SDL_free((void *)old);
                return NULL;
            }

            return old;
        }

        prev = node;
        node = node->next;
    }

    GenericHashMapNode *new_node = SDL_calloc(1, sizeof(GenericHashMapNode));
    SDL_assert(new_node != NULL);
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;

    if (prev)
    {
        prev->next = new_node;
    }
    else
    {
        map->buckets[idx] = new_node;
    }

    map->size++;
    return NULL;
}

const void *gen_hash_map_remove(GenericHashMap *map, const void *key)
{
    SDL_assert(map != NULL && key != NULL);
    SDL_assert(map->hash != NULL && map->comparator != NULL && map->buckets != NULL);

    uint64_t idx = map->hash(key) % map->capacity;

    GenericHashMapNode *prev = NULL;
    GenericHashMapNode *cur = map->buckets[idx];
    while (cur && map->comparator(cur->key, key) != 0)
    {
        prev = cur;
        cur = cur->next;
    }

    if (!cur)
    {
        return NULL;
    }

    if (prev)
    {
        prev->next = cur->next;
    }
    else
    {
        map->buckets[idx] = cur->next;
    }

    const void *value = cur->value;
    gen_hash_map_node_destroy(map, cur);
    map->size--;
    return map->destroys_value ? NULL : value;
}

bool gen_hash_map_has(const GenericHashMap *map, const void *key)
{
    SDL_assert(map != NULL && key != NULL);
    SDL_assert(map->hash != NULL && map->comparator != NULL);

    uint64_t idx = map->hash(key) % map->capacity;
    GenericHashMapNode *cur = map->buckets[idx];
    while (cur && map->comparator(cur->key, key) != 0)
    {
        cur = cur->next;
    }

    return cur != NULL;
}

const void *gen_hash_map_get(GenericHashMap *map, const void *key)
{
    SDL_assert(map != NULL && key != NULL);
    SDL_assert(map->hash != NULL && map->comparator != NULL);

    uint64_t idx = map->hash(key) % map->capacity;
    GenericHashMapNode *cur = map->buckets[idx];
    while (cur && map->comparator(cur->key, key) != 0)
    {
        cur = cur->next;
    }

    return cur ? cur->value : NULL;
}

void gen_hash_map_clear(GenericHashMap *map)
{
    SDL_assert(map != NULL);

    for (uint32_t i = 0; i < map->capacity; i++)
    {
        GenericHashMapNode *cur = map->buckets[i];
        while (cur)
        {
            GenericHashMapNode *next = cur->next;
            gen_hash_map_node_destroy(map, cur);
            cur = next;
        }
        map->buckets[i] = NULL;
    }
    map->size = 0;
}

void gen_hash_map_destroy(GenericHashMap *map)
{
    if (!map)
    {
        return;
    }

    gen_hash_map_clear(map);
    SDL_free(map->buckets);
    SDL_free(map);
}
