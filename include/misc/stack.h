// misc/stack.h
//
// Represents a LIFO stack.

#pragma once

#include <stdbool.h>

/**
 * Represents a LIFO stack.
 */
typedef struct
{
    void **items;
    int length;
    int capacity;
} Stack;

/**
 * Initialize a stack.
 */
Stack *stack_init(int capacity);

/**
 * Pushes an item into the stack in order. Returns false is the push failed.
 */
bool stack_push(Stack *stack, void *item);

/**
 * Peeks into the first element of the stack. Returns NULL if stack is empty.
 */
void *stack_peek(Stack *stack);

/**
 * Pops the first element of the stack. Returns NULL if stack is empty.
 */
void *stack_pop(Stack *stack);

/**
 * Clears the stack.
 */
void stack_clear(Stack *stack);

/**
 * Destroys a stack. The pointer provided is no longer deferencable.
 */
void stack_destroy(Stack *stack);
