#pragma once

#include "2d_body.h"

#define MG_PHYSICS_2D_MAX_BODIES 512

typedef struct mg_physics_world_2d mg_physics_world_2d_t;
struct mg_physics_world_2d
{
    mg_vec2_t gravity;
};