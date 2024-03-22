#include "opengl_pipeline.h"
#include "opengl_renderer.h"

#include <stdlib.h>

static GLenum mg_opengl_get_blend_factor(mg_blend_factor_t factor)
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
    case MG_BLEND_FACTOR_SRC1_COLOR: return GL_SRC1_COLOR;
    case MG_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR: return GL_ONE_MINUS_SRC1_COLOR;
    case MG_BLEND_FACTOR_SRC1_ALPHA: return GL_SRC1_ALPHA;
    case MG_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA: return GL_ONE_MINUS_SRC1_ALPHA;
}
}

static GLenum mg_opengl_get_blend_op(mg_blend_op_t blend_op)
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
        case MG_BLEND_OP_XOR: return GL_XOR;
        case MG_BLEND_OP_INVERT: return GL_INVERT;
        case MG_BLEND_OP_RED: return GL_RED;
        case MG_BLEND_OP_GREEN: return GL_GREEN;
        case MG_BLEND_OP_BLUE: return GL_BLUE;
        default: return GL_FUNC_ADD;
    }
}

static GLenum mg_opengl_get_polygon_mode(mg_polygon_mode_t mode)
{
    switch (mode)
    {
        case MG_POLYGON_MODE_FILL: return GL_TRIANGLES;
        case MG_POLYGON_MODE_LINE: return GL_LINE_LOOP;
        case MG_POLYGON_MODE_POINT: return GL_POINTS;
    }
}

static GLenum mg_opengl_get_cull_mode(mg_cull_mode_t mode)
{
    switch (mode)
    {
        case MG_CULL_MODE_NONE:
        glDisable(GL_CULL_FACE);
        return GL_NONE;
        case MG_CULL_MODE_FRONT:
        glEnable(GL_CULL_FACE);
        return GL_FRONT;
        case MG_CULL_MODE_BACK:
        glEnable(GL_CULL_FACE);
        return GL_BACK;
        case MG_CULL_MODE_FRONT_AND_BACK:
        glEnable(GL_CULL_FACE);
        return GL_FRONT_AND_BACK;
    }
}

GLenum mg_opengl_get_compare_op(mg_compare_op_t compare_op)
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
}

static GLenum mg_opengl_get_front_face(mg_front_face_t front_face)
{
    switch (front_face)
    {
        case MG_FRONT_FACE_CCW: return GL_CW;
        case MG_FRONT_FACE_CW: return GL_CCW;
    }
}

mg_opengl_pipeline_t *mg_opengl_create_pipeline(mg_pipeline_create_info_t *create_info)
{
    mg_opengl_pipeline_t *pipeline = (mg_opengl_pipeline_t*)malloc(sizeof(mg_opengl_pipeline_t));

    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &create_info->vertex_shader.code, NULL);
    glCompileShader(vertex_shader);
    
    /*int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }*/

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &create_info->fragment_shader.code, NULL);
    glCompileShader(fragment_shader);
    
    /*glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }*/

    pipeline->program_id = glCreateProgram();
    glAttachShader(pipeline->program_id, vertex_shader);
    glAttachShader(pipeline->program_id, fragment_shader);
    glLinkProgram(pipeline->program_id);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    pipeline->vertex_layout.stride = create_info->vertex_layout.stride;
    pipeline->vertex_layout.attribute_count = create_info->vertex_layout.attribute_count;
    pipeline->vertex_layout.attributes =
        malloc(create_info->vertex_layout.attribute_count * sizeof(mg_vertex_attribute_info_t));

    pipeline->color_blend = create_info->color_blend;
    pipeline->polygon_mode = create_info->polygon_mode;
    pipeline->cull_mode = create_info->cull_mode;
    pipeline->front_face = create_info->front_face;
    pipeline->depth_stencil = create_info->depth_stencil;

    for (uint32_t i = 0; i < create_info->vertex_layout.attribute_count; i++)
        pipeline->vertex_layout.attributes[i] = create_info->vertex_layout.attributes[i];
    
    pipeline->has_push_constants = create_info->has_push_constants;
    if (create_info->has_push_constants)
    {
        glGenBuffers(1, &pipeline->push_constant_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, pipeline->push_constant_buffer);
        glBufferData(GL_UNIFORM_BUFFER, create_info->push_constants_size, NULL, GL_STATIC_DRAW);
    }

    return pipeline;
}

void mg_opengl_destroy_pipeline(mg_opengl_pipeline_t *pipeline)
{
    glDeleteProgram(pipeline->program_id);

    if (pipeline->has_push_constants)
        glDeleteBuffers(1, &pipeline->push_constant_buffer);

    free(pipeline->vertex_layout.attributes);
    free(pipeline);
}

void mg_opengl_bind_pipeline(mg_opengl_pipeline_t *pipeline)
{
    opengl_context.polygon_mode = mg_opengl_get_polygon_mode(pipeline->polygon_mode);

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

    glCullFace(mg_opengl_get_cull_mode(pipeline->cull_mode));
    glFrontFace(mg_opengl_get_front_face(pipeline->front_face));

    if (pipeline->depth_stencil.depth_test_enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (pipeline->depth_stencil.stencil_test_enable)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
    
    pipeline->depth_stencil.depth_compare_op =
        mg_opengl_get_compare_op(pipeline->depth_stencil.depth_compare_op);

    glUseProgram(pipeline->program_id);

    opengl_context.current_pipeline = pipeline;
}