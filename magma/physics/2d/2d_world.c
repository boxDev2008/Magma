#include "2d_world.h"

#include <stdlib.h>

typedef struct mg_physics_world_2d mg_physics_world_2d_t;
struct mg_physics_world_2d
{
    mg_vec2_t gravity;
    mg_physics_body_2d_t *bodies;
    uint32_t max_bodies;
};

static mg_physics_world_2d_t physics_world;

void mg_physics_world_2d_initialize(mg_vec2_t gravity, uint32_t max_bodies)
{
    physics_world.bodies = (mg_physics_body_2d_t*)malloc(sizeof(mg_physics_body_2d_t) * max_bodies);
    physics_world.max_bodies = max_bodies;
}

void mg_physics_world_2d_shutdown(void)
{
    free(physics_world.bodies);
}

uint32_t mg_physics_world_2d_find_free_index(void)
{
    for (uint32_t i = 0; i < physics_world.max_bodies; i++)
        if (!physics_world.bodies[i].is_created)
            return i;
}

mg_physics_body_2d_t *mg_physics_world_2d_create_rect_body(mg_vec4_t aabb)
{
    uint32_t id = mg_physics_world_2d_find_free_index();
    mg_physics_body_2d_t *body = &physics_world.bodies[id];
    body->type = MG_PHYSICS_BODY_TYPE_RECT;
    body->rect_info.aabb = aabb;
    body->is_created = true;
    return body;
}

mg_physics_body_2d_t *mg_physics_world_2d_create_circle_body(float radius)
{
    uint32_t id = mg_physics_world_2d_find_free_index();
    mg_physics_body_2d_t *body = &physics_world.bodies[id];
    body->type = MG_PHYSICS_BODY_TYPE_CIRCLE;
    body->circle_info.radius = radius;
    body->is_created = true;
    return body;
}

void mg_physics_world_2d_destroy_body(mg_physics_body_2d_t *body)
{
    body->is_created = false;
}

int mg_physics_world_2d_check_rect_collision(mg_vec4_t a, mg_vec4_t b)
{
    return a.x < b.x + b.z && a.x + a.z > b.x &&
           a.y < b.y + b.w && a.y + a.w > b.y;
}

void mg_physics_world_2d_step(float delta_time)
{
    for (uint32_t i = 0; i < physics_world.max_bodies; i++)
    {
        if (!physics_world.bodies[i].is_created)
            continue;

        for (uint32_t j = 0; j < physics_world.max_bodies; j++)
        {
            if (i == j || !physics_world.bodies[j].is_created)
                continue;

            mg_physics_body_2d_t *body1 = &physics_world.bodies[i];
            mg_physics_body_2d_t *body2 = &physics_world.bodies[j];

            
            if (body1->type == MG_PHYSICS_BODY_TYPE_RECT &&
                body2->type == MG_PHYSICS_BODY_TYPE_RECT)
                mg_physics_world_2d_check_rect_collision(body1->rect_info.aabb, body2->rect_info.aabb);
            /*else if (body1->type == MG_PHYSICS_BODY_TYPE_RECT &&
                    body2->type == MG_PHYSICS_BODY_TYPE_CIRCLE)
            {
                
            }
            else if (body1->type == MG_PHYSICS_BODY_TYPE_CIRCLE &&
                    body2->type == MG_PHYSICS_BODY_TYPE_CIRCLE)
            {
                
            }*/
        }
    }
}