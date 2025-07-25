#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define array(type, capacity) struct { \
    unsigned int count; \
    type data[capacity]; \
}

#define array_push(arr, val) \
    arr.data[arr.count++] = val

#define array_pop(arr) \
    arr.data[--arr.count]

#define array_remove(arr, index) \
    array_remove_internal(&arr, index, sizeof(arr.data[0]))

#define array_index_of(arr, val_ptr) \
    ((val_ptr) - arr.data)

#define array_clear(arr) (arr.count = 0)

void array_remove_internal(void *arr, unsigned int index, size_t element_size);

#ifdef __cplusplus
}
#endif