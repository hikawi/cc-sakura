#include "misc/list.h"

#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"

List *list_init(void)
{
    List *list     = SDL_malloc(sizeof(List));
    list->capacity = 10;
    list->items    = SDL_malloc(sizeof(void *) * list->capacity);
    list->length   = 0;
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

    Uint32 new_capacity    = list->capacity / 2;
    const void **new_items = SDL_realloc(list->items, sizeof(void *) * new_capacity);
    if (new_items)
    {
        list->items    = new_items;
        list->capacity = new_capacity;
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Reallocation for list failed.");
    }
}

void list_add(List *list, const void *item)
{
    if (list->length + 1 <= list->capacity)
        list_expand(list);
    list->items[list->length++] = item;
}

void list_add_at(List *list, const void *item, const Uint32 k)
{
    if (k >= list->length)
        return;
    if (list->length + 1 <= list->capacity)
        list_expand(list);

    list_expand(list);
    SDL_memmove(list->items + k + 1, list->items + k, sizeof(void *) * (list->length - k));
    list->items[k] = item;
    list->length++;
}

void list_remove(List *list, const void *item)
{
    int idx = list_find(list, item);
    if (idx >= 0)
        list_remove_at(list, (Uint32)idx);
}

void list_remove_at(List *list, const Uint32 idx)
{
    if (idx >= list->length)
        return;

    if (idx < list->length - 1)
    {
        SDL_memmove(&list->items[idx], &list->items[idx + 1], sizeof(void *) * (list->length - idx - 1));
    }
    list->length--;
    list_shrink(list);
}

const void *list_get(const List *list, const int index)
{
    if (index < 0 || index >= (int)list->length)
        return NULL;
    return list->items[index];
}

int list_find(const List *list, const void *item)
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

bool list_has(const List *list, const void *item)
{
    return list_find(list, item) >= 0;
}

void list_join(List *list, const List *src)
{
    if (src == NULL || src->length == 0)
        return;
    while (list->length + src->length > list->capacity)
        list_expand(list);

    SDL_memcpy(&list->items[list->length], src->items, sizeof(void *) * src->length);
    list->length += src->length;
}

void list_clear(List *list)
{
    list->length = 0;
    list_shrink(list);
}

void quick_sort(const void **array, int start, int end, int (*comparator)(const void *, const void *))
{
    if (start >= end - 1)
        return;

    // Partition.
    const void *pivot = array[start];
    int i = start - 1, j = end;
    while (true)
    {
        do
        {
            i++;
        } while (comparator(array[i], pivot) < 0);
        do
        {
            j--;
        } while (comparator(array[j], pivot) > 0);

        if (i >= j)
            break;

        const void *tmp = array[i];
        array[i]        = array[j];
        array[j]        = tmp;
    }

    // Conquer
    quick_sort(array, start, j, comparator);
    quick_sort(array, j + 1, end, comparator);
}

void list_sort(List *list, int (*const comparator)(const void *, const void *))
{
    // Ig we use quick sort?
    quick_sort(list->items, 0, (int)list->length, comparator);
}

void list_destroy(List *list)
{
    SDL_free(list->items);
    SDL_free(list);
}
