#include "stack.h"

#include <stdlib.h>
#include <string.h>

void mg_create_stack(mg_stack *stack, uint32_t element_size, uint32_t capacity)
{
    stack->buffer = (uint8_t*)malloc(capacity * element_size);
    stack->capacity = capacity;
    stack->element_size = element_size;
    stack->pos = 0;
}

void mg_destroy_stack(mg_stack *stack)
{
    free(stack->buffer);
}

void mg_stack_push(mg_stack *stack, const void *data)
{
    if (stack->pos >= stack->capacity)
        return;
    memcpy(stack->buffer + (stack->pos++ * stack->element_size), data, stack->element_size);
}

void *mg_stack_pop(mg_stack *stack)
{
    void *result = stack->buffer + ((--stack->pos) * stack->element_size);
    return result;
}

bool mg_is_stack_empty(mg_stack *stack)
{
    return stack->pos == 0;
}