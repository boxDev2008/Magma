#include "random.h"
#include <stdlib.h>

float mg_randomf(void)
{
    return (float)rand() / (float)RAND_MAX;
}

float mg_random_rangef(float min, float max)
{
    return min + (max - min) * mg_randomf();
}

int mg_random_rangei(int min, int max)
{
    return min + (max - min) * mg_randomf();
}