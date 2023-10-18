#pragma once

#include "base.h"

#include "math/vec2.h"
#include "math/vec4.h"

typedef enum mg_physics_body_type_2d mg_physics_body_type_2d_t;
enum mg_physics_body_type_2d
{
    MG_PHYSICS_BODY_TYPE_AABB,
    MG_PHYSICS_BODY_TYPE_CIRCLE
};

typedef struct mg_physics_body_2d mg_physics_body_2d_t;
struct mg_physics_body_2d
{
    bool is_created;
    bool is_static;

    uint32_t id;

    mg_physics_body_type_2d_t type;
    mg_vec2_t position;
    mg_vec2_t velocity;
    mg_vec2_t fall_speed;
    float mass;

    struct
    {
        mg_vec4_t aabb;
    }
    aabb_info;

    struct
    {
        float radius;
    }
    circle_info;
};

MG_API void mg_physics_world_2d_initialize(mg_vec2_t gravity, uint32_t max_bodies);
MG_API void mg_physics_world_2d_shutdown(void);
MG_API void mg_physics_world_2d_step(float delta_time);

MG_API mg_physics_body_2d_t    *mg_physics_world_2d_create_body(void);
MG_API void                     mg_physics_world_2d_destroy_body(mg_physics_body_2d_t *body);