#include "opengl_pipeline.h"
#include "opengl_renderer.h"

#include <stdlib.h>

static GLenum mg_opengl_get_blend_factor(mg_blend_factor factor)
{
    switch (factor)
    {
        case MG_BLEND_FACTOR_ZERO: return GL_ZERO;
        case MG_BLEND_FACTOR_ONE: return GL_ONE;
        case MG_BLEND_FACTOR_SRC_COLOR: return GL_SRC_COLOR;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
        case MG_BLEND_FACTOR_DST_COLOR: return GL_DST_COLOR;
        case MG_BLEND_FACTOR_ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
        case MG_BLEND_FACTOR_SRC_ALPHA: return GL_SRC_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        case MG_BLEND_FACTOR_DST_ALPHA: return GL_DST_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
        case MG_BLEND_FACTOR_CONSTANT_COLOR: return GL_CONSTANT_COLOR;
        case MG_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
        case MG_BLEND_FACTOR_CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
        case MG_BLEND_FACTOR_SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
#if !MG_PLATFORM_EMSCRIPTEN
        case MG_BLEND_FACTOR_SRC1_COLOR: return GL_SRC1_COLOR;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR: return GL_ONE_MINUS_SRC1_COLOR;
        case MG_BLEND_FACTOR_SRC1_ALPHA: return GL_SRC1_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA: return GL_ONE_MINUS_SRC1_ALPHA;
#endif
    }

    return GL_ZERO;
}

static GLenum mg_opengl_get_blend_op(mg_blend_op blend_op)
{
    switch (blend_op)
    {
        case MG_BLEND_OP_ADD: return GL_FUNC_ADD;
        case MG_BLEND_OP_SUBTRACT: return GL_FUNC_SUBTRACT;
        case MG_BLEND_OP_REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
        case MG_BLEND_OP_MIN: return GL_MIN;
        case MG_BLEND_OP_MAX: return GL_MAX;
        case MG_BLEND_OP_ZERO: return GL_ZERO;
        case MG_BLEND_OP_SRC: return GL_SRC_COLOR;
        case MG_BLEND_OP_DST: return GL_DST_COLOR;
        case MG_BLEND_OP_INVERT: return GL_INVERT;
        case MG_BLEND_OP_RED: return GL_RED;
        case MG_BLEND_OP_GREEN: return GL_GREEN;
        case MG_BLEND_OP_BLUE: return GL_BLUE;
#if !MG_PLATFORM_EMSCRIPTEN
        case MG_BLEND_OP_XOR: return GL_XOR;
#endif
        default: return GL_FUNC_ADD;
    }
}

#if !MG_PLATFORM_EMSCRIPTEN
static GLenum mg_opengl_get_polygon_mode(mg_polygon_mode mode)
{
    switch (mode)
    {
        case MG_POLYGON_MODE_FILL: return GL_FILL;
        case MG_POLYGON_MODE_LINE: return GL_LINE;
        case MG_POLYGON_MODE_POINT: return GL_POINT;
    }
    return GL_FILL;
}
#endif

static GLenum mg_opengl_get_primitive_topology(mg_primitive_topology topology)
{
    switch (topology)
    {
        case MG_PRIMITIVE_TOPOLOGY_POINT_LIST: return GL_POINTS;
        case MG_PRIMITIVE_TOPOLOGY_LINE_LIST: return GL_LINES;
        case MG_PRIMITIVE_TOPOLOGY_LINE_STRIP: return GL_LINE_STRIP;
        case MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: return GL_TRIANGLES;
        case MG_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
        case MG_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN: return GL_TRIANGLE_FAN;
    }
    return GL_POINTS;
}

static GLenum mg_opengl_get_cull_mode(mg_cull_mode mode)
{
    switch (mode)
    {
        case MG_CULL_MODE_NONE:
        return GL_NONE;
        case MG_CULL_MODE_FRONT:
        return GL_FRONT;
        case MG_CULL_MODE_BACK:
        return GL_BACK;
        case MG_CULL_MODE_FRONT_AND_BACK:
        return GL_FRONT_AND_BACK;
    }
    return GL_NONE;
}

GLenum mg_opengl_get_compare_op(mg_compare_op compare_op)
{
    switch (compare_op)
    {
        case MG_COMPARE_OP_NEVER: return GL_NEVER;
        case MG_COMPARE_OP_LESS: return GL_LESS;
        case MG_COMPARE_OP_EQUAL: return GL_EQUAL;
        case MG_COMPARE_OP_LESS_OR_EQUAL: return GL_LEQUAL;
        case MG_COMPARE_OP_GREATER: return GL_GREATER;
        case MG_COMPARE_OP_NOT_EQUAL: return GL_NOTEQUAL;
        case MG_COMPARE_OP_GREATER_OR_EQUAL: return GL_GEQUAL;
        case MG_COMPARE_OP_ALWAYS: return GL_ALWAYS;
    }
    return GL_NEVER;
}

static GLenum mg_opengl_get_front_face(mg_front_face front_face)
{
    switch (front_face)
    {
        case MG_FRONT_FACE_CCW: return GL_CCW;
        case MG_FRONT_FACE_CW: return GL_CW;
    }
    return GL_CW;
}

static void mg_opengl_fill_graphics_pipeline(mg_opengl_pipeline *pipeline, mg_pipeline_create_info *create_info)
{
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar *const *)&create_info->shader.vertex.code, (const GLint *)&create_info->shader.vertex.size);
    glCompileShader(vertex_shader);

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const GLchar *const *)&create_info->shader.fragment.code, (const GLint *)&create_info->shader.fragment.size);
    glCompileShader(fragment_shader);

    pipeline->program_id = glCreateProgram();
    glAttachShader(pipeline->program_id, vertex_shader);
    glAttachShader(pipeline->program_id, fragment_shader);
    glLinkProgram(pipeline->program_id);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(pipeline->program_id);

    for (uint32_t i = 0; i < MG_CONFIG_MAX_BINDABLE_UNIFORMS && create_info->shader.uniform_blocks[i].name; i++)
    {
        const uint32_t index = glGetUniformBlockIndex(pipeline->program_id, create_info->shader.uniform_blocks[i].name);   
        glUniformBlockBinding(pipeline->program_id, index, create_info->shader.uniform_blocks[i].binding);
    }

    for (uint32_t i = 0; i < MG_CONFIG_MAX_BINDABLE_IMAGES && create_info->shader.sampled_images[i].name; i++)
    {
        const GLint index = glGetUniformLocation(pipeline->program_id, create_info->shader.sampled_images[i].name);
        glUniform1iv(index, 1, &create_info->shader.sampled_images[i].binding);
    }

    pipeline->vertex_layout.stride = create_info->vertex_layout.stride;

    uint32_t attribute_count = 0;
    for (attribute_count = 0;
        attribute_count < MG_CONFIG_MAX_VERTEX_ATTRIBUTES &&
        create_info->vertex_layout.attributes[attribute_count].format; attribute_count++)
        pipeline->vertex_layout.attributes[attribute_count] = create_info->vertex_layout.attributes[attribute_count];
    pipeline->vertex_layout.attribute_count = attribute_count;

    pipeline->color_blend = create_info->color_blend;
#if !MG_PLATFORM_EMSCRIPTEN
    pipeline->polygon_mode = mg_opengl_get_polygon_mode(create_info->polygon_mode);
#endif
    pipeline->primitive_topology = mg_opengl_get_primitive_topology(create_info->primitive_topology);
    pipeline->cull_mode = mg_opengl_get_cull_mode(create_info->cull_mode);
    pipeline->front_face = mg_opengl_get_front_face(create_info->front_face);

    pipeline->depth_stencil.depth_test_enabled = create_info->depth_stencil.depth_test_enabled;
    pipeline->depth_stencil.stencil_test_enabled = create_info->depth_stencil.stencil_test_enabled;
    pipeline->depth_stencil.depth_write_enabled = create_info->depth_stencil.depth_write_enabled;
    pipeline->depth_stencil.depth_compare_op = mg_opengl_get_compare_op(create_info->depth_stencil.depth_compare_op);

    pipeline->type = MG_OPENGL_PIPELINE_TYPE_GRAPHICS;
}

static void mg_opengl_fill_compute_pipeline(mg_opengl_pipeline *pipeline, mg_pipeline_create_info *create_info)
{
    uint32_t compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, (const GLchar *const *)&create_info->shader.compute.code, (const GLint *)&create_info->shader.compute.size);
    glCompileShader(compute_shader);

    pipeline->program_id = glCreateProgram();
    glAttachShader(pipeline->program_id, compute_shader);
    glLinkProgram(pipeline->program_id);

    glDeleteShader(compute_shader);

    pipeline->type = MG_OPENGL_PIPELINE_TYPE_COMPUTE;
}

mg_opengl_pipeline *mg_opengl_create_pipeline(mg_pipeline_create_info *create_info)
{
    mg_opengl_pipeline *pipeline = (mg_opengl_pipeline*)malloc(sizeof(mg_opengl_pipeline));
    if (create_info->shader.compute.size)
        mg_opengl_fill_compute_pipeline(pipeline, create_info);
    else mg_opengl_fill_graphics_pipeline(pipeline, create_info);
    return pipeline;
}

void mg_opengl_destroy_pipeline(mg_opengl_pipeline *pipeline)
{
    glDeleteProgram(pipeline->program_id);
    free(pipeline);
}

void mg_opengl_bind_pipeline(mg_opengl_pipeline *pipeline)
{
    glUseProgram(pipeline->program_id);

    if (pipeline->type == MG_OPENGL_PIPELINE_TYPE_COMPUTE)
        return;

    gl_ctx.primitive_topology = pipeline->primitive_topology;
    gl_ctx.current_pipeline = pipeline;

    if (pipeline->color_blend.blend_enabled)
    {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(mg_opengl_get_blend_factor(pipeline->color_blend.src_color_blend_factor),
            mg_opengl_get_blend_factor(pipeline->color_blend.dst_color_blend_factor),
            mg_opengl_get_blend_factor(pipeline->color_blend.src_alpha_blend_factor),
            mg_opengl_get_blend_factor(pipeline->color_blend.dst_alpha_blend_factor));
        glBlendEquationSeparate(mg_opengl_get_blend_op(pipeline->color_blend.color_blend_op),
            mg_opengl_get_blend_op(pipeline->color_blend.alpha_blend_op));
    }
    else
        glDisable(GL_BLEND);

    if (pipeline->cull_mode == GL_NONE)
        glDisable(GL_CULL_FACE);
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(pipeline->cull_mode);
    }
    glFrontFace(pipeline->front_face);
#if !MG_PLATFORM_EMSCRIPTEN
    glPolygonMode(GL_FRONT_AND_BACK, pipeline->polygon_mode);
#endif
    pipeline->depth_stencil.depth_test_enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    pipeline->depth_stencil.stencil_test_enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    glDepthFunc(pipeline->depth_stencil.depth_compare_op);
    glDepthMask(pipeline->depth_stencil.depth_write_enabled);
}