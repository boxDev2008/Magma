#pragma once

#include "body_2d.h"

MG_API void mg_physics_world_2d_initialize(mg_vec2_t gravity, uint32_t max_bodies);
MG_API void mg_physics_world_2d_shutdown(void);
MG_API void mg_physics_world_2d_step(float delta_time);

MG_API mg_physics_body_2d_t *mg_physics_world_2d_create_rect_body(mg_vec4_t aabb);
MG_API mg_physics_body_2d_t *mg_physics_world_2d_create_circle_body(float radius);
MG_API void mg_physics_world_2d_destroy_body(mg_physics_body_2d_t *body);