#pragma once

#include "base.h"

typedef struct mg_stack
{
    uint8_t *buffer;
    uint32_t capacity;
    uint32_t element_size;
    uint32_t pos;
}
mg_stack;

MG_API void     mg_create_stack     (mg_stack *stack, uint32_t element_size, uint32_t capacity);
MG_API void     mg_destroy_stack    (mg_stack *stack);
MG_API void     mg_stack_push       (mg_stack *stack, const void *data);
MG_API void    *mg_stack_pop        (mg_stack *stack);

MG_API bool     mg_is_stack_empty   (mg_stack *stack);