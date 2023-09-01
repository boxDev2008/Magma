#pragma once

#include "platform/platform.h"
#include "math/vec2.h"

#define MG_MAX_VERTEX_ATTRIBUTES 16
#define MG_MAX_DESCRIPTORS 16
#define MG_MAX_DESCRIPTOR_SET_LAYOUTS 8

typedef void *mg_handle;

typedef enum mg_renderer_type mg_renderer_type_t;
enum mg_renderer_type
{
    MG_RENDERER_TYPE_OPENGL,
    MG_RENDERER_TYPE_VULKAN
};

typedef enum mg_buffer_usage mg_buffer_usage_t;
enum mg_buffer_usage
{
    MG_BUFFER_USAGE_UNIFORM = 0x00000010,
    MG_BUFFER_USAGE_STORAGE = 0x00000020,
    MG_BUFFER_USAGE_INDEX = 0x00000040,
    MG_BUFFER_USAGE_VERTEX = 0x00000080
};

typedef enum mg_index_type mg_index_type_t;
enum mg_index_type
{
    MG_INDEX_TYPE_UINT16 = 0,
    MG_INDEX_TYPE_UINT32 = 1
};

typedef struct mg_buffer_create_info mg_buffer_create_info_t;
struct mg_buffer_create_info
{
    mg_buffer_usage_t usage;
    size_t size;

    bool mapped_at_creation;
};

typedef struct mg_buffer_update_info mg_buffer_update_info_t;
struct mg_buffer_update_info
{
    size_t size;
    void *data;
};

typedef struct mg_buffer mg_buffer_t;
struct mg_buffer
{
    mg_handle internal_data;
};

typedef enum mg_primitive_topology mg_primitive_topology_t;
enum mg_primitive_topology
{
    MG_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    MG_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    MG_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
    MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    MG_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4
};

typedef enum mg_polygon_mode mg_polygon_mode_t;
enum mg_polygon_mode
{
    MG_POLYGON_MODE_FILL = 0,
    MG_POLYGON_MODE_LINE = 1,
    MG_POLYGON_MODE_POINT = 2
};

typedef enum mg_cull_mode mg_cull_mode_t;
enum mg_cull_mode
{
    MG_CULL_MODE_NONE = 0,
    MG_CULL_MODE_FRONT = 0x00000001,
    MG_CULL_MODE_BACK = 0x00000002,
    MG_CULL_MODE_FRONT_AND_BACK = 0x00000003
};

typedef enum mg_front_face mg_front_face_t;
enum mg_front_face
{
    MG_FRONT_FACE_CCW = 0,
    MG_FRONT_FACE_CW = 1
};

typedef enum mg_descriptor_type mg_descriptor_type_t;
enum mg_descriptor_type
{
    MG_DESCRIPTOR_TYPE_SAMPLER = 0,
    MG_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    MG_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    MG_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    MG_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    MG_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7
};
 
typedef struct mg_descriptor mg_descriptor_t;
struct mg_descriptor
{
    mg_descriptor_type_t type;
    uint32_t binding;
};

typedef struct mg_descriptor_set_layout_create_info mg_descriptor_set_layout_create_info_t;
struct mg_descriptor_set_layout_create_info
{
    mg_descriptor_t descriptors[MG_MAX_DESCRIPTORS];
    uint32_t descriptor_count;
};

typedef struct mg_descriptor_set_layout mg_descriptor_set_layout_t;
struct mg_descriptor_set_layout
{
    mg_handle internal_data;
};

typedef struct mg_descriptor_set_create_info mg_descriptor_set_create_info_t;
struct mg_descriptor_set_create_info
{
    mg_descriptor_set_layout_t layouts[MG_MAX_DESCRIPTOR_SET_LAYOUTS];
    uint32_t layout_count;
};

typedef struct mg_descriptor_set mg_descriptor_set_t;
struct mg_descriptor_set
{
    mg_handle internal_data;
};

typedef enum mg_vertex_format mg_vertex_format_t;
enum mg_vertex_format
{
    MG_VERTEX_FORMAT_UINT = 98,
    MG_VERTEX_FORMAT_INT = 99,
    MG_VERTEX_FORMAT_FLOAT = 100,

    MG_VERTEX_FORMAT_UINT2 = 101,
    MG_VERTEX_FORMAT_INT2 = 102,
    MG_VERTEX_FORMAT_FLOAT2 = 103,

    MG_VERTEX_FORMAT_UINT3 = 104,
    MG_VERTEX_FORMAT_INT3 = 105,
    MG_VERTEX_FORMAT_FLOAT3 = 106,

    MG_VERTEX_FORMAT_UINT4 = 107,
    MG_VERTEX_FORMAT_INT4 = 108,
    MG_VERTEX_FORMAT_FLOAT4 = 109
};

typedef struct mg_vertex_attribute_info mg_vertex_attribute_info_t;
struct mg_vertex_attribute_info
{
    uint32_t location;
    mg_vertex_format_t format;
    uint32_t offset;
};

typedef struct mg_vertex_layout_info mg_vertex_layout_info_t;
struct mg_vertex_layout_info
{
    uint32_t stride;
    mg_vertex_attribute_info_t *attributes;
    uint32_t attribute_count;
};

/*typedef enum mg_shader_type mg_shader_type_t;
enum mg_shader_type
{
	MG_SHADER_TYPE_VERTEX = 0x00000001,
	MG_SHADER_TYPE_GEOMETRY = 0x00000008,
	MG_SHADER_TYPE_FRAGMENT = 0x00000010,
	MG_SHADER_TYPE_COMPUTE = 0x00000020
};*/

typedef struct mg_shader_source mg_shader_source_t;
struct mg_shader_source
{
	uint32_t* code;
	size_t code_size;
};

typedef struct mg_program_create_info mg_program_create_info_t;
struct mg_program_create_info
{
    mg_shader_source_t vertex_shader;
    mg_shader_source_t fragment_shader;

    mg_vertex_layout_info_t vertex_layout;

    mg_primitive_topology_t primitive_topology;
    mg_polygon_mode_t polygon_mode;
    mg_cull_mode_t cull_mode;
    mg_front_face_t front_face;
    
    mg_descriptor_set_layout_t desctriptor_set_layouts[MG_MAX_DESCRIPTOR_SET_LAYOUTS];
    uint32_t desctriptor_set_layout_count;

    bool has_push_constants;
    uint16_t push_constants_size;
};

typedef struct mg_program mg_program_t;
struct mg_program
{
    mg_handle internal_data;
};

MG_API void mg_llapi_renderer_initialize    (mg_platform_t *platform, mg_renderer_type_t type);
MG_API void mg_llapi_renderer_shutdown      (void);

MG_API void mg_llapi_renderer_resize        (int32_t width, int32_t height);

MG_API void mg_llapi_renderer_begin_frame   (void);
MG_API void mg_llapi_renderer_end_frame     (void);

MG_API void mg_llapi_renderer_present_frame (void);

MG_API void mg_llapi_renderer_wait          (void);

MG_API void mg_llapi_renderer_draw          (uint32_t vertex_count, uint32_t first_vertex);
MG_API void mg_llapi_renderer_draw_indexed  (uint32_t vertex_index, uint32_t first_index);

MG_API mg_descriptor_set_layout_t   mg_llapi_renderer_create_descriptor_set_layout  (mg_descriptor_set_layout_create_info_t *create_info);
MG_API mg_descriptor_set_t          mg_llapi_renderer_create_descriptor_set         (mg_descriptor_set_create_info_t *create_info);
MG_API void                         mg_llapi_renderer_update_descriptor_set         (mg_descriptor_set_t descriptor_set); // TODO (box): Add update info struct similar to VkWriteDescriptorSet

MG_API mg_program_t                 mg_llapi_renderer_create_program                (mg_program_create_info_t *create_info);
MG_API void                         mg_llapi_renderer_destroy_program               (mg_program_t program);
MG_API void                         mg_llapi_renderer_bind_program                  (mg_program_t program);

MG_API mg_buffer_t                  mg_llapi_renderer_create_buffer                 (mg_buffer_create_info_t *create_info);
MG_API void                         mg_llapi_renderer_update_buffer                 (mg_buffer_t buffer, mg_buffer_update_info_t *update_info);
MG_API void                         mg_llapi_renderer_destroy_buffer                (mg_buffer_t buffer);

MG_API void                         mg_llapi_renderer_bind_vertex_buffer            (mg_buffer_t buffer);
MG_API void                         mg_llapi_renderer_bind_index_buffer             (mg_buffer_t buffer, mg_index_type_t index_type);