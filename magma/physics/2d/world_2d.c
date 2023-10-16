#include "world_2d.h"

#include <stdlib.h>
#include <math.h>

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
    physics_world.gravity = gravity;
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

bool mg_physics_2d_aabb_vs_aabb(mg_vec4_t a, mg_vec4_t b)
{
    if (a.z < b.x || a.x > b.z) return false;
    if (a.w < b.y || a.y > b.w) return false;
    return true;
}

/*bool isBody1AboveCenterOfBody2(mg_physics_body_2d_t *body1, mg_physics_body_2d_t *body2)
{
    mg_vec2_t center1 = {
        (body1->rect_info.aabb.x + body1->rect_info.aabb.z) / 2.0f,
        (body1->rect_info.aabb.y + body1->rect_info.aabb.w) / 2.0f
    };
    mg_vec2_t center2 = {
        (body2->rect_info.aabb.x + body2->rect_info.aabb.z) / 2.0f,
        (body2->rect_info.aabb.y + body2->rect_info.aabb.w) / 2.0f
    };

    return center1.y > center2.y;
}*/

void mg_physics_2d_resolve_aabb_vs_aabb_collision(mg_physics_body_2d_t *body1, mg_physics_body_2d_t *body2)
{
    if (body1->is_static)
        return;

    mg_vec4_t aabb1 = mg_vec4_add(body1->rect_info.aabb, (mg_vec4_t){ body1->position.x, body1->position.y, body1->position.x, body1->position.y });
    mg_vec4_t aabb2 = mg_vec4_add(body2->rect_info.aabb, (mg_vec4_t){ body2->position.x, body2->position.y, body2->position.x, body2->position.y });

    if (!mg_physics_2d_aabb_vs_aabb(aabb1, aabb2))
        return;

    float overlap_x = fminf(aabb1.z, aabb2.z) - fmaxf(aabb1.x, aabb2.x);
    float overlap_y = fminf(aabb1.w, aabb2.w) - fmaxf(aabb1.y, aabb2.y);

    if (fabs(overlap_x) < fabs(overlap_y))
    {
        body1->position.x -= overlap_x;
        body1->velocity.x = 0;
    }
    else
    {
        body1->position.y -= overlap_y;
        body1->velocity.y = 0;
    }
}

bool mg_physics_2d_cirlce_vs_cirlce(mg_vec2_t a, float ar, mg_vec2_t b, float br)
{
    float r = ar + br;
    return r < mg_vec2_distance(a, b);
}

void mg_physics_2d_resolve_circle_vs_circle_collision(mg_physics_body_2d_t *body1, mg_physics_body_2d_t *body2)
{
    if (!mg_physics_2d_cirlce_vs_cirlce(body1->position, body1->circle_info.radius, body2->position, body2->circle_info.radius))
        return;
}

void mg_physics_world_2d_step_movement(float delta_time)
{
    for (uint32_t i = 0; i < physics_world.max_bodies; i++)
    {
        if (!physics_world.bodies[i].is_created || physics_world.bodies[i].is_static)
            continue;

        mg_physics_body_2d_t *body = &physics_world.bodies[i];

        body->velocity.x += physics_world.gravity.x * body->fall_speed.x;
        body->velocity.y += physics_world.gravity.y * body->fall_speed.y;

        body->position.x += body->velocity.x;
        body->position.y += body->velocity.y;
    }
}

void mg_physics_world_2d_step_collision(float delta_time)
{
    for (uint32_t i = 0; i < physics_world.max_bodies; i++)
    {
        if (!physics_world.bodies[i].is_created)
            continue;

        mg_physics_body_2d_t *body1 = &physics_world.bodies[i];

        for (uint32_t j = i + 1; j < physics_world.max_bodies; j++)
        {
            if (!physics_world.bodies[j].is_created)
                continue;

            mg_physics_body_2d_t *body2 = &physics_world.bodies[j];
            
            if (body1->type == MG_PHYSICS_BODY_TYPE_RECT &&
                body2->type == MG_PHYSICS_BODY_TYPE_RECT)
                mg_physics_2d_resolve_aabb_vs_aabb_collision(body1, body2);
            else if (body1->type == MG_PHYSICS_BODY_TYPE_CIRCLE &&
                    body2->type == MG_PHYSICS_BODY_TYPE_CIRCLE)
                mg_physics_2d_resolve_circle_vs_circle_collision(body1, body2);
            /*else if (body1->type == MG_PHYSICS_BODY_TYPE_RECT &&
                    body2->type == MG_PHYSICS_BODY_TYPE_CIRCLE)
            {
                
            }*/
        }
    }
}

void mg_physics_world_2d_step(float delta_time)
{
    mg_physics_world_2d_step_movement(delta_time);
    mg_physics_world_2d_step_collision(delta_time);
}