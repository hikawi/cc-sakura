#include "misc/list.h"
#include "SDL3/SDL_stdinc.h"

List *list_init(void)
{
    List *list = SDL_malloc(sizeof(List));
    list->capacity = 10;
    list->items = SDL_malloc(sizeof(void *) * list->capacity);
    list->length = 0;
    return list;
}

void list_expand(List *list)
{
    list->items = SDL_realloc(list->items, sizeof(void *) * list->capacity * 2);
    list->capacity *= 2;
}

void list_shrink(List *list)
{
    if (list->capacity / 4 <= list->length)
        return;

    list->items = SDL_realloc(list->items, sizeof(void *) * list->capacity / 2);
    list->capacity /= 2;
}

void list_add(List *list, void *item)
{
    if (list->length + 1 <= list->capacity)
        list_expand(list);
    list->items[list->length++] = item;
}

void list_add_at(List *list, void *item, Uint32 k)
{
    if (k < 0 || k >= list->length)
        return;
    if (list->length + 1 <= list->capacity)
        list_expand(list);

    list_expand(list);
    SDL_memmove(list->items + k + 1, list->items + k,
                sizeof(void *) * (list->length - k));
    list->items[k] = item;
    list->length++;
}

void list_remove(List *list, void *item)
{
    int idx = list_find(list, item);
    if (idx >= 0)
        list_remove_at(list, (Uint32)idx);
}

void list_remove_at(List *list, Uint32 idx)
{
    if (idx >= list->length)
        return;

    if (idx < list->length - 1)
    {
        SDL_memmove(&list->items[idx], &list->items[idx + 1],
                    sizeof(void *) * (list->length - idx - 1));
    }
    list->length--;
    list_shrink(list);
}

void *list_get(List *list, int index)
{
    if (index < 0 || index >= (int)list->length)
        return NULL;
    return list->items[index];
}

int list_find(List *list, void *item)
{
    for (Uint32 i = 0; i < list->length; i++)
    {
        if (list->items[i] == item)
        {
            return (int)i;
        }
    }
    return -1;
}

bool list_has(List *list, void *item)
{
    return list_find(list, item) >= 0;
}

void list_join(List *list, List *src)
{
    if (src == NULL || src->length == 0)
        return;
    while (list->length + src->length > list->capacity)
        list_expand(list);

    SDL_memcpy(&list->items[list->length], src->items,
               sizeof(void *) * src->length);
    list->length += src->length;
}

void list_clear(List *list)
{
    list->length = 0;
    list_shrink(list);
}

void list_destroy(List *list)
{
    SDL_free(list->items);
    SDL_free(list);
}
