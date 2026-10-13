#define MG_IMPL
#include "mgapp.h"

#define MGFX_VULKAN
#include "mgfx.h"

#include "mgmath.h"

#include "raytrace.glsl.h"
#include "present.glsl.h"

#include <string.h>

#define HISTORY_FORMAT MGFX_FORMAT_RGBA32_SFLOAT

static mgfx_pipeline raytrace_pip;
static mgfx_pipeline present_pip;

static mgfx_image raytrace_img;
static mgfx_image present_smp;

static mgfx_image color_hist[2];
static mgfx_image geo_hist[2];
static int hist_idx;

typedef struct
{
    vec3 origin; float _pad0;
    vec3 forward; float _pad1;
    vec3 right; float _pad2;
    vec3 up; float fov_scale;
} CamUBO;

typedef struct
{
    uint32_t frame_index;
    uint32_t reset_history;
    uint32_t camera_moved;
    uint32_t _pad0;
} FrameUBO;

static FrameUBO frame_ubo = { .reset_history = 1 };
static CamUBO prev_cam_ubo;

static CamUBO cam_look_at(vec3 eye, vec3 target, float fov_y)
{
    CamUBO c = {0};
    c.origin = eye;
    c.forward = vec3_norm(vec3_sub(target, eye));
    c.right = vec3_norm(vec3_cross(c.forward, (vec3){0, 1, 0}));
    c.up = vec3_cross(c.right, c.forward);
    c.fov_scale = tanf(fov_y * 0.5f);
    return c;
}

static CamUBO cam_yaw_pitch(vec3 eye, float yaw, float pitch, float fov_y)
{
    pitch = MG_CLAMP(pitch, -89.0f * MG_DEG2RAD, 89.0f * MG_DEG2RAD);
    float cp = cosf(pitch);
    vec3 f = { cp * sinf(yaw), sinf(pitch), -cp * cosf(yaw) };
    return cam_look_at(eye, vec3_add(eye, f), fov_y);
}

typedef struct {
    vec3 pos;
    float yaw, pitch;
    float fov_y;
    float speed;
    float sensitivity;
    int32_t last_mx, last_my;
} FlyCam;

static vec3 flycam_forward(const FlyCam *c)
{
    float cp = cosf(c->pitch);
    return (vec3){ cp * sinf(c->yaw), sinf(c->pitch), -cp * cosf(c->yaw) };
}

static vec3 flycam_right(const FlyCam *c)
{
    return (vec3){ cosf(c->yaw), 0.0f, sinf(c->yaw) };
}

static void flycam_update(FlyCam *c, float dt)
{
    dt = MG_MIN(dt, 0.1f);

    int32_t mx = mgapp_mouse_x(), my = mgapp_mouse_y();
    if (mgapp_mouse_down(MG_MOUSE_BUTTON_RIGHT))
    {
        c->yaw += (float)(mx - c->last_mx) * c->sensitivity;
        c->pitch -= (float)(my - c->last_my) * c->sensitivity;
        c->pitch = MG_CLAMP(c->pitch, -89.0f * MG_DEG2RAD, 89.0f * MG_DEG2RAD);
    }
    c->last_mx = mx;
    c->last_my = my;

    int8_t scroll = mgapp_mouse_scroll_delta();
    if (scroll)
        c->speed = MG_CLAMP(c->speed * powf(1.25f, (float)scroll), 0.1f, 100.0f);

    vec3 f = flycam_forward(c), r = flycam_right(c), up = {0, 1, 0};
    vec3 move = {0, 0, 0};
    if (mgapp_key_down(MG_KEY_W)) move = vec3_add(move, f);
    if (mgapp_key_down(MG_KEY_S)) move = vec3_sub(move, f);
    if (mgapp_key_down(MG_KEY_D)) move = vec3_add(move, r);
    if (mgapp_key_down(MG_KEY_A)) move = vec3_sub(move, r);
    if (mgapp_key_down(MG_KEY_E)) move = vec3_add(move, up);
    if (mgapp_key_down(MG_KEY_Q)) move = vec3_sub(move, up);

    float boost = (mgapp_key_down(MG_KEY_LSHIFT) ||
                   mgapp_key_down(MG_KEY_RSHIFT)) ? 4.0f : 1.0f;
    move = vec3_norm(move);
    c->pos = vec3_add(c->pos, vec3_scale(move, c->speed * boost * dt));
}

static CamUBO flycam_ubo(const FlyCam *c)
{
    CamUBO u = {0};
    u.origin = c->pos;
    u.forward = flycam_forward(c);
    u.right = flycam_right(c);
    u.up = vec3_cross(u.right, u.forward);
    u.fov_scale = tanf(c->fov_y * 0.5f);
    return u;
}

static void create_targets(void)
{
    raytrace_img = mgfx_create_image(&(mgfx_image_create_info){
        .width = mgapp_width(),
        .height = mgapp_height(),
        .type = MGFX_IMAGE_TYPE_2D,
        .format = MGFX_FORMAT_RGBA8_UNORM,
        .usage = MGFX_IMAGE_USAGE_STORAGE | MGFX_IMAGE_USAGE_SAMPLED
    });

    for (int i = 0; i < 2; i++)
    {
        color_hist[i] = mgfx_create_image(&(mgfx_image_create_info){
            .width = mgapp_width(),
            .height = mgapp_height(),
            .type = MGFX_IMAGE_TYPE_2D,
            .format = HISTORY_FORMAT,
            .usage = MGFX_IMAGE_USAGE_STORAGE
        });

        geo_hist[i] = mgfx_create_image(&(mgfx_image_create_info){
            .width = mgapp_width(),
            .height = mgapp_height(),
            .type = MGFX_IMAGE_TYPE_2D,
            .format = HISTORY_FORMAT,
            .usage = MGFX_IMAGE_USAGE_STORAGE
        });
    }

    hist_idx = 0;
    frame_ubo.reset_history = 1;
}

static void destroy_targets(void)
{
    mgfx_destroy_image(raytrace_img);

    for (int i = 0; i < 2; i++)
    {
        mgfx_destroy_image(color_hist[i]);
        mgfx_destroy_image(geo_hist[i]);
    }
}

void on_start(void)
{
    mgfx_init(&(mgfx_init_info){
        .primary_handle = mgapp_primary_handle(),
        .secondary_handle = mgapp_secondary_handle(),
        .width = mgapp_width(),
        .height = mgapp_height(),
        .vsync = true
    });

    raytrace_pip = mgfx_create_pipeline(&(mgfx_pipeline_create_info){
        .shader = get_raytrace_gi_shader(mgfx_get_shader_lang())
    });

    present_pip = mgfx_create_pipeline(&(mgfx_pipeline_create_info){
        .primitive_topology = MGFX_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        .shader = get_present_shader(mgfx_get_shader_lang())
    });

    create_targets();

    present_smp = mgfx_create_sampler(&(mgfx_sampler_create_info){
        .min_filter = MGFX_SAMPLER_FILTER_LINEAR,
        .mag_filter = MGFX_SAMPLER_FILTER_LINEAR
    });
}

void on_end(void)
{
    destroy_targets();
    mgfx_destroy_sampler(present_smp);
    mgfx_destroy_pipeline(present_pip);
    mgfx_destroy_pipeline(raytrace_pip);
    mgfx_shutdown();
}

static FlyCam cam = {
    .pos = {0, 0.5f, 2.0f},
    .yaw = 0.0f,
    .pitch = -0.1f,
    .fov_y = 60.0f * MG_DEG2RAD,
    .speed = 3.0f,
    .sensitivity = 0.0025f,
};

void on_update(void)
{
    if (mgfx_begin() != MGFX_RESULT_SUCCESS)
        return;

    flycam_update(&cam, mgapp_delta_time());
    CamUBO ubo = flycam_ubo(&cam);
    frame_ubo.camera_moved = memcmp(&ubo, &prev_cam_ubo, sizeof ubo) != 0;

    int cur = hist_idx;
    int prev = hist_idx ^ 1;

    mgfx_bind_pass(&(mgfx_pass_info){
        .clear = {0.0f, 0.0f, 0.0f, 1.0f}
    });
    mgfx_bind_pipeline(raytrace_pip);

    mgfx_bind_storage_image(raytrace_img, MGFX_ACCESS_READ_WRITE, 0);
    mgfx_bind_storage_image(color_hist[prev], MGFX_ACCESS_READ_WRITE, 1);
    mgfx_bind_storage_image(color_hist[cur], MGFX_ACCESS_READ_WRITE, 2);
    mgfx_bind_storage_image(geo_hist[prev], MGFX_ACCESS_READ_WRITE, 3);
    mgfx_bind_storage_image(geo_hist[cur], MGFX_ACCESS_READ_WRITE, 4);

    mgfx_bind_uniforms(0, sizeof(ubo), &ubo);
    mgfx_bind_uniforms(1, sizeof(frame_ubo), &frame_ubo);
    mgfx_bind_uniforms(2, sizeof(prev_cam_ubo), &prev_cam_ubo);

    mgfx_dispatch(
        (mgapp_width() + 7) / 8,
        (mgapp_height() + 7) / 8,
        1
    );

    mgfx_bind_pipeline(present_pip);
    mgfx_bind_sampled_image(raytrace_img, present_smp, 0);
    mgfx_draw(4, 0);
    mgfx_end();

    prev_cam_ubo = ubo;
    hist_idx ^= 1;
    frame_ubo.frame_index++;
    frame_ubo.reset_history = 0;
}

void on_event(const mgapp_event *event)
{
    if (event->type == MGAPP_EVENT_RESIZE)
    {
        destroy_targets();
        create_targets();
        mgfx_resize(event->window.width, event->window.height);
    }
}

mgapp_init_info mgapp_main(void)
{
    return (mgapp_init_info) {
        .title = "Raytrace",
        .events = {
            .start = on_start,
            .end = on_end,
            .update = on_update,
            .event = on_event
        }
    };
}