#pragma once

#include "base.h"

#include "math/vec2.h"
#include "math/vec4.h"

typedef enum mg_physics_body_type_2d mg_physics_body_type_2d_t;
enum mg_physics_body_type_2d
{
    MG_PHYSICS_BODY_TYPE_RECT,
    MG_PHYSICS_BODY_TYPE_CIRCLE
};

typedef struct mg_physics_body_2d mg_physics_body_2d_t;
struct mg_physics_body_2d
{
    bool is_created;

    mg_physics_body_type_2d_t type;
    mg_vec2_t position;
    mg_vec2_t velocity;
    float mass;
    float fall_speed;

    struct
    {
        mg_vec4_t aabb;
    }
    rect_info;

    struct
    {
        float radius;
    }
    circle_info;
};