#include "misc/stack.h"
#include "SDL3/SDL_stdinc.h"

Stack *stack_init(int capacity)
{
    Stack *stack = SDL_malloc(sizeof(Stack));
    stack->items = SDL_calloc((Uint32)capacity, sizeof(void *));
    stack->capacity = capacity;
    stack->length = 0;
    return stack;
}

bool stack_push(Stack *stack, void *item)
{
    if (stack->length >= stack->capacity)
        return false;

    stack->items[stack->length++] = item;
    return true;
}

void *stack_peek(Stack *stack)
{
    if (stack->length == 0)
        return NULL;
    return stack->items[stack->length - 1];
}

void *stack_pop(Stack *stack)
{
    if (stack->length == 0)
        return NULL;
    void *item = stack->items[stack->length - 1];
    stack->length--;
    return item;
}

void stack_clear(Stack *stack)
{
    stack->length = 0;
}

void stack_destroy(Stack *stack)
{
    if (!stack)
        return;

    SDL_free(stack->items);
    SDL_free(stack);
}
