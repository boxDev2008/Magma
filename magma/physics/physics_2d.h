#pragma once

#include "base.h"

#include "math/vec2.h"
#include "math/vec4.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MG_CONFIG_PHYSICS_2D_BODIES
    #define MG_CONFIG_PHYSICS_2D_BODIES 256
#endif

typedef enum mg_physics_body_type_2d
{
    MG_PHYSICS_BODY_TYPE_AABB,
    MG_PHYSICS_BODY_TYPE_CIRCLE
}
mg_physics_body_type_2d_t;

typedef struct mg_physics_body_info_2d
{
    bool is_static;

    mg_physics_body_type_2d_t type;
    mg_vec2_t position;
    mg_vec2_t velocity;
    mg_vec2_t fall_speed;
    float restitution;
    float mass;

    uint8_t layer;
    uint64_t collidable_layers;

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
}
mg_physics_body_info_2d_t;

typedef struct mg_physics_body_2d
{
    uint32_t id;

    mg_physics_body_info_2d_t info;

    bool is_created;
    bool is_grounded;
}
mg_physics_body_2d_t;

MG_API void mg_physics_world_2d_initialize  (mg_vec2_t gravity);
MG_API void mg_physics_world_2d_shutdown    (void);
MG_API void mg_physics_world_2d_step        (float delta_time);

MG_API mg_physics_body_2d_t    *mg_physics_world_2d_create_body     (mg_physics_body_info_2d_t *create_info);
MG_API void                     mg_physics_world_2d_destroy_body    (mg_physics_body_2d_t *body);

#ifdef __cplusplus
}
#endif