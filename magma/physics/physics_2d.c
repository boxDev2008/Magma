#include "physics_2d.h"

#define CUTE_C2_IMPLEMENTATION
#include <cute_c2.h>

#include <stdlib.h>
#include <stdio.h>

typedef struct mg_physics_world_2d mg_physics_world_2d_t;
struct mg_physics_world_2d
{
    mg_vec2_t gravity;
    mg_physics_body_2d_t bodies[MG_CONFIG_PHYSICS_2D_BODIES];
};

static mg_physics_world_2d_t *physics_world;

void mg_physics_world_2d_initialize(mg_vec2_t gravity)
{
    physics_world = (mg_physics_world_2d_t*)malloc(sizeof(mg_physics_world_2d_t));
    physics_world->gravity = gravity;

    for (uint32_t i = 0; i < MG_CONFIG_PHYSICS_2D_BODIES; i++)
        physics_world->bodies[i].is_created = false;
}

void mg_physics_world_2d_shutdown(void)
{
    free(physics_world);
}

uint32_t mg_physics_world_2d_find_free_index(void)
{
    for (uint32_t i = 0; i < MG_CONFIG_PHYSICS_2D_BODIES; i++)
        if (!physics_world->bodies[i].is_created)
            return i;
}

mg_physics_body_2d_t *mg_physics_world_2d_create_body(mg_physics_body_info_2d_t *create_info)
{
    uint32_t id = mg_physics_world_2d_find_free_index();
    mg_physics_body_2d_t *body = &physics_world->bodies[id];
    body->info = *create_info;
    body->is_created = true;
    return body;
}

void mg_physics_world_2d_destroy_body(mg_physics_body_2d_t *body)
{
    body->is_created = false;
}

void mg_physics_2d_resolve_aabb_vs_aabb_collision(mg_physics_body_2d_t *body1, mg_physics_body_2d_t *body2)
{
    if (body1->info.is_static || !(body1->info.collidable_layers & body2->info.layer))
        return;

    c2AABB aabb1, aabb2;
    aabb1.min = c2V(body1->info.aabb_info.aabb.x + body1->info.position.x, body1->info.aabb_info.aabb.y + body1->info.position.y);
    aabb1.max = c2V(body1->info.aabb_info.aabb.z + body1->info.position.x, body1->info.aabb_info.aabb.w + body1->info.position.y);
    aabb2.min = c2V(body2->info.aabb_info.aabb.x + body2->info.position.x, body2->info.aabb_info.aabb.y + body2->info.position.y);
    aabb2.max = c2V(body2->info.aabb_info.aabb.z + body2->info.position.x, body2->info.aabb_info.aabb.w + body2->info.position.y);

    if (!c2AABBtoAABB(aabb1, aabb2))
        return;

    c2Manifold manifold;
    c2AABBtoAABBManifold(aabb1, aabb2, &manifold);

    if (manifold.n.x != 0.0f)
    {
        body1->info.position.x -= manifold.n.x * manifold.depths[0];
        body1->info.velocity.x = (body1->info.velocity.x - manifold.n.x) * body1->info.restitution;
    }

    if (manifold.n.y != 0.0f)
    {
        body1->is_grounded = manifold.n.y > 0.0f;
        body1->info.position.y -= manifold.n.y * manifold.depths[0];
        body1->info.velocity.y = (body1->info.velocity.y - manifold.n.y) * body1->info.restitution;
    }
}

void mg_physics_2d_resolve_circle_vs_circle_collision(mg_physics_body_2d_t *body1, mg_physics_body_2d_t *body2)
{
    if (body1->info.is_static && !(body1->info.collidable_layers & body2->info.layer))
        return;

    c2Circle c1, c2;
    c1.p = c2V(body1->info.position.x, body1->info.position.y);
    c1.r = body1->info.circle_info.radius;
    c2.p = c2V(body2->info.position.x, body2->info.position.y);
    c2.r = body2->info.circle_info.radius;

    if (!c2CircletoCircle(c1, c2))
        return;

    c2Manifold manifold;
    c2CircletoCircleManifold(c1, c2, &manifold);

    if (manifold.n.x != 0.0f)
    {
        body1->info.position.x -= manifold.n.x * manifold.depths[0];
        body1->info.velocity.x = (body1->info.velocity.x - manifold.n.x) * body1->info.restitution;
    }

    if (manifold.n.y != 0.0f)
    {
        body1->is_grounded = manifold.n.y > 0.0f;
        body1->info.position.y -= manifold.n.y * manifold.depths[0];
        body1->info.velocity.y = (body1->info.velocity.y - manifold.n.y) * body1->info.restitution;
    }
}

void mg_physics_2d_resolve_circle_vs_aabb_collision(mg_physics_body_2d_t *body1, mg_physics_body_2d_t *body2)
{
    if (body1->info.is_static && !(body1->info.collidable_layers & body2->info.layer))
        return;

    c2Circle c;
    c.p = c2V(body1->info.position.x, body1->info.position.y);
    c.r = body1->info.circle_info.radius;

    c2AABB aabb;
    aabb.min = c2V(body2->info.aabb_info.aabb.x + body2->info.position.x, body2->info.aabb_info.aabb.y + body2->info.position.y);
    aabb.max = c2V(body2->info.aabb_info.aabb.z + body2->info.position.x, body2->info.aabb_info.aabb.w + body2->info.position.y);

    if (!c2CircletoAABB(c, aabb))
        return;

    c2Manifold manifold;
    c2CircletoAABBManifold(c, aabb, &manifold);

    if (manifold.n.x != 0.0f)
    {
        body1->info.position.x -= manifold.n.x * manifold.depths[0];
        body1->info.velocity.x = (body1->info.velocity.x - manifold.n.x) * body1->info.restitution;
    }

    if (manifold.n.y != 0.0f)
    {
        body1->is_grounded = manifold.n.y > 0.0f;
        body1->info.position.y -= manifold.n.y * manifold.depths[0];
        body1->info.velocity.y = (body1->info.velocity.y - manifold.n.y) * body1->info.restitution;
    }
}

void mg_physics_2d_resolve_aabb_vs_circle_collision(mg_physics_body_2d_t *body1, mg_physics_body_2d_t *body2)
{
    if (body1->info.is_static && !(body1->info.collidable_layers & body2->info.layer))
        return;

    c2AABB aabb;
    aabb.min = c2V(body1->info.aabb_info.aabb.x + body1->info.position.x, body1->info.aabb_info.aabb.y + body1->info.position.y);
    aabb.max = c2V(body1->info.aabb_info.aabb.z + body1->info.position.x, body1->info.aabb_info.aabb.w + body1->info.position.y);

    c2Circle c;
    c.p = c2V(body2->info.position.x, body2->info.position.y);
    c.r = body2->info.circle_info.radius;

    if (!c2CircletoAABB(c, aabb))
        return;

    c2Manifold manifold;
    c2CircletoAABBManifold(c, aabb, &manifold);

    if (manifold.n.x != 0.0f)
    {
        body1->info.position.x += manifold.n.x * manifold.depths[0];
        body1->info.velocity.x = (body1->info.velocity.x + manifold.n.x) * body1->info.restitution;
    }

    if (manifold.n.y != 0.0f)
    {
        body1->is_grounded = manifold.n.y < 0.0f;
        body1->info.position.y += manifold.n.y * manifold.depths[0];
        body1->info.velocity.y = (body1->info.velocity.y + manifold.n.y) * body1->info.restitution;
    }
}

void mg_physics_world_2d_step_movement(float delta_time)
{
    for (uint32_t i = 0; i < MG_CONFIG_PHYSICS_2D_BODIES; i++)
    {
        if (!physics_world->bodies[i].is_created || physics_world->bodies[i].info.is_static)
            continue;

        mg_physics_body_2d_t *body = &physics_world->bodies[i];

        body->info.velocity.x += physics_world->gravity.x * body->info.fall_speed.x * delta_time;
        body->info.velocity.y += physics_world->gravity.y * body->info.fall_speed.y * delta_time;

        body->info.position.x += body->info.velocity.x * delta_time;
        body->info.position.y += body->info.velocity.y * delta_time;
    }
}

void mg_physics_world_2d_step_collision(float delta_time)
{    
    for (uint32_t i = 0; i < MG_CONFIG_PHYSICS_2D_BODIES; i++)
    {
        if (!physics_world->bodies[i].is_created)
            continue;

        mg_physics_body_2d_t *body1 = &physics_world->bodies[i];
        body1->is_grounded = false;

        for (uint32_t j = 0; j < MG_CONFIG_PHYSICS_2D_BODIES; j++)
        {
            if (!physics_world->bodies[j].is_created || i == j)
                continue;

            mg_physics_body_2d_t *body2 = &physics_world->bodies[j];
            
            if (body1->info.type == MG_PHYSICS_BODY_TYPE_AABB &&
                body2->info.type == MG_PHYSICS_BODY_TYPE_AABB)
                mg_physics_2d_resolve_aabb_vs_aabb_collision(body1, body2);
            else if (body1->info.type == MG_PHYSICS_BODY_TYPE_CIRCLE &&
                    body2->info.type == MG_PHYSICS_BODY_TYPE_CIRCLE)
                mg_physics_2d_resolve_circle_vs_circle_collision(body1, body2);
            else if (body1->info.type == MG_PHYSICS_BODY_TYPE_AABB &&
                    body2->info.type == MG_PHYSICS_BODY_TYPE_CIRCLE)
                mg_physics_2d_resolve_aabb_vs_circle_collision(body1, body2);
            else if (body1->info.type == MG_PHYSICS_BODY_TYPE_CIRCLE &&
                    body2->info.type == MG_PHYSICS_BODY_TYPE_AABB)
                mg_physics_2d_resolve_circle_vs_aabb_collision(body1, body2);
        }
    }
}

void mg_physics_world_2d_step(float delta_time)
{
    mg_physics_world_2d_step_movement(delta_time);
    mg_physics_world_2d_step_collision(delta_time);
}