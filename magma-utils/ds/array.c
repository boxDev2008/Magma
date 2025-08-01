#include "array.h"
#include <string.h>

void array_remove_internal(void *arr, unsigned int index, size_t element_size)
{
    unsigned int *count_ptr = (unsigned int *)arr;
    unsigned int count = *count_ptr;

    if (index >= count)
        return;

    void *data = (char *)arr + sizeof(unsigned int);
    void *dst = (char *)data + index * element_size;
    void *src = (char *)data + (index + 1) * element_size;

    size_t move_size = (count - index - 1) * element_size;
    memmove(dst, src, move_size);

    *count_ptr = count - 1;
}