#include "misc/hashset.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"

HashSet *hash_set_init(void)
{
    HashSet *set = SDL_calloc(1, sizeof(HashSet));
    return set;
}

bool hash_set_add(HashSet *set, void *item)
{
    if (!set || !item)
    {
        return false;
    }

    if (!set->hash)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Can't use hash set without a hash function.");
        return false;
    }

    Uint32 hash = set->hash(item) % HASH_SET_MAX_BUCKETS;

    // First node.
    if (!set->buckets[hash])
    {
        set->buckets[hash] = SDL_malloc(sizeof(HashSetNode));
        set->buckets[hash]->item = item;
        set->buckets[hash]->next = NULL;
        set->length++;
        return true;
    }

    // Find the end of the node, or ignore if already added.
    HashSetNode *cur = set->buckets[hash];
    HashSetNode *prev = NULL;
    while (cur)
    {
        if ((set->compare && set->compare(item, cur->item)) ||
            cur->item == item)
        {
            return false; // already added
        }

        prev = cur;
        cur = cur->next;
    }

    // Found the end, but not added.
    HashSetNode *node = SDL_malloc(sizeof(HashSetNode));
    node->item = item;
    node->next = NULL;
    prev->next = node;
    set->length++;
    return true;
}

bool hash_set_remove(HashSet *set, void *item)
{
    if (!set || !item)
    {
        return false;
    }

    if (!set->hash)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Can't use hash set without a hash function.");
        return false;
    }

    Uint32 hash = set->hash(item) % HASH_SET_MAX_BUCKETS;

    // Find the end of the node, or ignore if already added.
    HashSetNode *cur = set->buckets[hash];
    HashSetNode *prev = NULL;
    while (cur)
    {
        if ((set->compare && set->compare(item, cur->item)) ||
            cur->item == item)
        {
            if (prev)
            {
                prev->next = cur->next;
            }
            else
            {
                set->buckets[hash] = cur->next;
            }

            SDL_free(cur);
            set->length--;
            return true;
        }

        prev = cur;
        cur = cur->next;
    }

    // Did not found
    return false;
}

bool hash_set_has(HashSet *set, void *item)
{
    if (!set || !item)
    {
        return false;
    }

    if (!set->hash)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Can't use hash set without a hash function.");
        return false;
    }

    Uint32 hash = set->hash(item) % HASH_SET_MAX_BUCKETS;

    // Find the end of the node, or ignore if already added.
    HashSetNode *cur = set->buckets[hash];
    while (cur)
    {
        if ((set->compare && set->compare(item, cur->item)) ||
            cur->item == item)
        {
            return true;
        }

        cur = cur->next;
    }

    // Did not found
    return false;
}

void hash_set_clear(HashSet *set)
{
    if (!set)
    {
        return;
    }

    for (int i = 0; i < HASH_SET_MAX_BUCKETS; i++)
    {
        HashSetNode *node = set->buckets[i];
        while (node)
        {
            HashSetNode *next = node->next;
            SDL_free(node);
            node = next;
        }
    }

    set->length = 0;
}

void hash_set_destroy(HashSet *set)
{
    hash_set_clear(set);
    SDL_free(set);
}
