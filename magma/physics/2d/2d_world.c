#include "2d_world.h"

#include <stdlib.h>

typedef struct mg_physics_world_2d mg_physics_world_2d_t;
struct mg_physics_world_2d
{
    mg_vec2_t gravity;
    mg_physics_body_2d_t *bodies;
};

static mg_physics_world_2d_t physics_world;

void mg_physics_world_2d_initialize(mg_vec2_t gravity, uint32_t max_bodies)
{
    physics_world.bodies = (mg_physics_body_2d_t*)malloc(sizeof(mg_physics_body_2d_t) * max_bodies);
}

void mg_physics_world_2d_shutdown(void)
{
    free(physics_world.bodies);
}

mg_physics_body_2d_t *mg_physics_world_2d_create_body(void)
{
    
}