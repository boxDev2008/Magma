#pragma once

#include "platform/platform.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MG_CONFIG_MAX_IMAGE_ARRAYS
    #define MG_CONFIG_MAX_IMAGE_ARRAYS 1024
#endif

#ifndef MG_CONFIG_MAX_BINDABLE_IMAGES
    #define MG_CONFIG_MAX_BINDABLE_IMAGES 8
#endif

#ifndef MG_CONFIG_MAX_BINDABLE_UNIFORMS
    #define MG_CONFIG_MAX_BINDABLE_UNIFORMS 4
#endif

#ifndef MG_CONFIG_MAX_VERTEX_ATTRIBUTES
    #define MG_CONFIG_MAX_VERTEX_ATTRIBUTES 8
#endif

#ifndef MG_CONFIG_MAX_UNIFORM_UPDATE_SIZE
    #define MG_CONFIG_MAX_UNIFORM_UPDATE_SIZE (1 << 16)
#endif

#ifndef MG_CONFIG_MAX_SCRATCH_BUFFER_SIZE
    #define MG_CONFIG_MAX_SCRATCH_BUFFER_SIZE (1 << 22)
#endif

typedef enum mg_renderer_type
{
    MG_RENDERER_TYPE_VULKAN,
    MG_RENDERER_TYPE_OPENGL,
    MG_RENDERER_TYPE_DIRECT3D11
}
mg_renderer_type;

typedef enum mg_index_type
{
    MG_INDEX_TYPE_UINT16 = 0,
    MG_INDEX_TYPE_UINT32 = 1
}
mg_index_type;

typedef void *mg_buffer,
*mg_vertex_buffer, *mg_index_buffer,
*mg_dynamic_vertex_buffer, *mg_dynamic_index_buffer;

typedef enum mg_pixel_format
{
    MG_PIXEL_FORMAT_R8_UNORM = 9,
    MG_PIXEL_FORMAT_R8_SNORM = 10,
    MG_PIXEL_FORMAT_R8_USCALED = 11,
    MG_PIXEL_FORMAT_R8_SSCALED = 12,
    MG_PIXEL_FORMAT_R8_UINT = 13,
    MG_PIXEL_FORMAT_R8_SINT = 14,
    MG_PIXEL_FORMAT_R8_SRGB = 15,
    MG_PIXEL_FORMAT_R8G8_UNORM = 16,
    MG_PIXEL_FORMAT_R8G8_SNORM = 17,
    MG_PIXEL_FORMAT_R8G8_USCALED = 18,
    MG_PIXEL_FORMAT_R8G8_SSCALED = 19,
    MG_PIXEL_FORMAT_R8G8_UINT = 20,
    MG_PIXEL_FORMAT_R8G8_SINT = 21,
    MG_PIXEL_FORMAT_R8G8_SRGB = 22,
    MG_PIXEL_FORMAT_R8G8B8_UNORM = 23,
    MG_PIXEL_FORMAT_R8G8B8_SNORM = 24,
    MG_PIXEL_FORMAT_R8G8B8_USCALED = 25,
    MG_PIXEL_FORMAT_R8G8B8_SSCALED = 26,
    MG_PIXEL_FORMAT_R8G8B8_UINT = 27,
    MG_PIXEL_FORMAT_R8G8B8_SINT = 28,
    MG_PIXEL_FORMAT_R8G8B8_SRGB = 29,
    MG_PIXEL_FORMAT_B8G8R8_UNORM = 30,
    MG_PIXEL_FORMAT_B8G8R8_SNORM = 31,
    MG_PIXEL_FORMAT_B8G8R8_USCALED = 32,
    MG_PIXEL_FORMAT_B8G8R8_SSCALED = 33,
    MG_PIXEL_FORMAT_B8G8R8_UINT = 34,
    MG_PIXEL_FORMAT_B8G8R8_SINT = 35,
    MG_PIXEL_FORMAT_B8G8R8_SRGB = 36,
    MG_PIXEL_FORMAT_R8G8B8A8_UNORM = 37,
    MG_PIXEL_FORMAT_R8G8B8A8_SNORM = 38,
    MG_PIXEL_FORMAT_R8G8B8A8_USCALED = 39,
    MG_PIXEL_FORMAT_R8G8B8A8_SSCALED = 40,
    MG_PIXEL_FORMAT_R8G8B8A8_UINT = 41,
    MG_PIXEL_FORMAT_R8G8B8A8_SINT = 42,
    MG_PIXEL_FORMAT_R8G8B8A8_SRGB = 43,
    MG_PIXEL_FORMAT_B8G8R8A8_UNORM = 44,
    MG_PIXEL_FORMAT_B8G8R8A8_SNORM = 45,
    MG_PIXEL_FORMAT_B8G8R8A8_USCALED = 46,
    MG_PIXEL_FORMAT_B8G8R8A8_SSCALED = 47,
    MG_PIXEL_FORMAT_B8G8R8A8_UINT = 48,
    MG_PIXEL_FORMAT_B8G8R8A8_SINT = 49,
    MG_PIXEL_FORMAT_B8G8R8A8_SRGB = 50,
    MG_PIXEL_FORMAT_R16_UNORM = 70,
    MG_PIXEL_FORMAT_R16_SNORM = 71,
    MG_PIXEL_FORMAT_R16_USCALED = 72,
    MG_PIXEL_FORMAT_R16_SSCALED = 73,
    MG_PIXEL_FORMAT_R16_UINT = 74,
    MG_PIXEL_FORMAT_R16_SINT = 75,
    MG_PIXEL_FORMAT_R16_SFLOAT = 76,
    MG_PIXEL_FORMAT_R16G16_UNORM = 77,
    MG_PIXEL_FORMAT_R16G16_SNORM = 78,
    MG_PIXEL_FORMAT_R16G16_USCALED = 79,
    MG_PIXEL_FORMAT_R16G16_SSCALED = 80,
    MG_PIXEL_FORMAT_R16G16_UINT = 81,
    MG_PIXEL_FORMAT_R16G16_SINT = 82,
    MG_PIXEL_FORMAT_R16G16_SFLOAT = 83,
    MG_PIXEL_FORMAT_R16G16B16_UNORM = 84,
    MG_PIXEL_FORMAT_R16G16B16_SNORM = 85,
    MG_PIXEL_FORMAT_R16G16B16_USCALED = 86,
    MG_PIXEL_FORMAT_R16G16B16_SSCALED = 87,
    MG_PIXEL_FORMAT_R16G16B16_UINT = 88,
    MG_PIXEL_FORMAT_R16G16B16_SINT = 89,
    MG_PIXEL_FORMAT_R16G16B16_SFLOAT = 90,
    MG_PIXEL_FORMAT_R16G16B16A16_UNORM = 91,
    MG_PIXEL_FORMAT_R16G16B16A16_SNORM = 92,
    MG_PIXEL_FORMAT_R16G16B16A16_USCALED = 93,
    MG_PIXEL_FORMAT_R16G16B16A16_SSCALED = 94,
    MG_PIXEL_FORMAT_R16G16B16A16_UINT = 95,
    MG_PIXEL_FORMAT_R16G16B16A16_SINT = 96,
    MG_PIXEL_FORMAT_R16G16B16A16_SFLOAT = 97,
    MG_PIXEL_FORMAT_R32_UINT = 98,
    MG_PIXEL_FORMAT_R32_SINT = 99,
    MG_PIXEL_FORMAT_R32_SFLOAT = 100,
    MG_PIXEL_FORMAT_R32G32_UINT = 101,
    MG_PIXEL_FORMAT_R32G32_SINT = 102,
    MG_PIXEL_FORMAT_R32G32_SFLOAT = 103,
    MG_PIXEL_FORMAT_R32G32B32_UINT = 104,
    MG_PIXEL_FORMAT_R32G32B32_SINT = 105,
    MG_PIXEL_FORMAT_R32G32B32_SFLOAT = 106,
    MG_PIXEL_FORMAT_R32G32B32A32_UINT = 107,
    MG_PIXEL_FORMAT_R32G32B32A32_SINT = 108,
    MG_PIXEL_FORMAT_R32G32B32A32_SFLOAT = 109,
    MG_PIXEL_FORMAT_R64_UINT = 110,
    MG_PIXEL_FORMAT_R64_SINT = 111,
    MG_PIXEL_FORMAT_R64_SFLOAT = 112,
    MG_PIXEL_FORMAT_R64G64_UINT = 113,
    MG_PIXEL_FORMAT_R64G64_SINT = 114,
    MG_PIXEL_FORMAT_R64G64_SFLOAT = 115,
    MG_PIXEL_FORMAT_R64G64B64_UINT = 116,
    MG_PIXEL_FORMAT_R64G64B64_SINT = 117,
    MG_PIXEL_FORMAT_R64G64B64_SFLOAT = 118,
    MG_PIXEL_FORMAT_R64G64B64A64_UINT = 119,
    MG_PIXEL_FORMAT_R64G64B64A64_SINT = 120,
    MG_PIXEL_FORMAT_R64G64B64A64_SFLOAT = 121,
    MG_PIXEL_FORMAT_D32_SFLOAT = 126,
    MG_PIXEL_FORMAT_D16_UNORM_S8_UINT = 128,
    MG_PIXEL_FORMAT_D24_UNORM_S8_UINT = 129,
    MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT = 130
}
mg_pixel_format;

typedef enum mg_image_type
{
    MG_IMAGE_TYPE_1D = 0,
    MG_IMAGE_TYPE_2D = 1,
    MG_IMAGE_TYPE_3D = 2,
    MG_IMAGE_TYPE_CUBE = 3
}
mg_image_type;

typedef enum mg_image_usage
{
    MG_IMAGE_USAGE_COLOR_ATTACHMENT = 16,
    MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 32
}
mg_image_usage;

typedef struct mg_image_create_info
{
    mg_pixel_format format;
    mg_image_type type;
    uint32_t usage;
    uint32_t width, height;
}
mg_image_create_info;

typedef void *mg_image;

typedef struct mg_image_write_info
{
    mg_pixel_format format;
    uint32_t width, height;
    void *data;
}
mg_image_write_info;

typedef enum mg_sampler_filter
{
    MG_SAMPLER_FILTER_NEAREST = 0,
    MG_SAMPLER_FILTER_LINEAR = 1
}
mg_sampler_filter;

typedef enum mg_sampler_address_mode
{
    MG_SAMPLER_ADDRESS_MODE_REPEAT = 0,
    MG_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
    MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
    MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
    MG_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = 4
}
mg_sampler_address_mode;

typedef struct mg_sampler_create_info
{
    mg_sampler_filter mag_filter;
    mg_sampler_filter min_filter;

    mg_sampler_address_mode address_mode_u;
    mg_sampler_address_mode address_mode_v;
    mg_sampler_address_mode address_mode_w;
}
mg_sampler_create_info;

typedef void *mg_sampler;
typedef void *mg_image_array;

typedef struct mg_attachment_info
{
    mg_pixel_format format;
}
mg_attachment_info;

typedef struct mg_render_pass_create_info
{
    mg_attachment_info color_attachment;
    mg_attachment_info depth_stencil_attachment;
    bool has_depth_stencil_attachment;
}
mg_render_pass_create_info;

typedef void *mg_render_pass;

typedef struct mg_framebuffer_create_info
{
    mg_render_pass render_pass;
    mg_image color_attachment;
    mg_image depth_stencil_attachment;
    uint32_t width, height;
}
mg_framebuffer_create_info;

typedef void *mg_framebuffer;

typedef struct mg_render_pass_begin_info
{
    mg_vec4 render_area;
    mg_vec4 clear_value;
}
mg_render_pass_begin_info;

typedef enum mg_primitive_topology
{
    MG_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    MG_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    MG_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
    MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    MG_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
    MG_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5
}
mg_primitive_topology;

typedef enum mg_polygon_mode
{
    MG_POLYGON_MODE_FILL = 0,
    MG_POLYGON_MODE_LINE = 1,
    MG_POLYGON_MODE_POINT = 2
}
mg_polygon_mode;

typedef enum mg_cull_mode
{
    MG_CULL_MODE_NONE = 0,
    MG_CULL_MODE_FRONT = 0x00000001,
    MG_CULL_MODE_BACK = 0x00000002,
    MG_CULL_MODE_FRONT_AND_BACK = 0x00000003
}
mg_cull_mode;

typedef enum mg_front_face
{
    MG_FRONT_FACE_CCW = 0,
    MG_FRONT_FACE_CW = 1
}
mg_front_face;

typedef enum mg_blend_factor
{
    MG_BLEND_FACTOR_ZERO = 0,
    MG_BLEND_FACTOR_ONE = 1,
    MG_BLEND_FACTOR_SRC_COLOR = 2,
    MG_BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 3,
    MG_BLEND_FACTOR_DST_COLOR = 4,
    MG_BLEND_FACTOR_ONE_MINUS_DST_COLOR = 5,
    MG_BLEND_FACTOR_SRC_ALPHA = 6,
    MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 7,
    MG_BLEND_FACTOR_DST_ALPHA = 8,
    MG_BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 9,
    MG_BLEND_FACTOR_CONSTANT_COLOR = 10,
    MG_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = 11,
    MG_BLEND_FACTOR_CONSTANT_ALPHA = 12,
    MG_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = 13,
    MG_BLEND_FACTOR_SRC_ALPHA_SATURATE = 14,
    MG_BLEND_FACTOR_SRC1_COLOR = 15,
    MG_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR = 16,
    MG_BLEND_FACTOR_SRC1_ALPHA = 17,
    MG_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA = 18
}
mg_blend_factor;

typedef enum mg_blend_op
{
    MG_BLEND_OP_ADD = 0,
    MG_BLEND_OP_SUBTRACT = 1,
    MG_BLEND_OP_REVERSE_SUBTRACT = 2,
    MG_BLEND_OP_MIN = 3,
    MG_BLEND_OP_MAX = 4,
    MG_BLEND_OP_ZERO = 1000148000,
    MG_BLEND_OP_SRC = 1000148001,
    MG_BLEND_OP_DST = 1000148002,
    MG_BLEND_OP_SRC_OVER = 1000148003,
    MG_BLEND_OP_DST_OVER = 1000148004,
    MG_BLEND_OP_SRC_IN = 1000148005,
    MG_BLEND_OP_DST_IN = 1000148006,
    MG_BLEND_OP_SRC_OUT = 1000148007,
    MG_BLEND_OP_DST_OUT = 1000148008,
    MG_BLEND_OP_SRC_ATOP = 1000148009,
    MG_BLEND_OP_DST_ATOP = 1000148010,
    MG_BLEND_OP_XOR = 1000148011,
    MG_BLEND_OP_MULTIPLY = 1000148012,
    MG_BLEND_OP_SCREEN = 1000148013,
    MG_BLEND_OP_OVERLAY = 1000148014,
    MG_BLEND_OP_DARKEN = 1000148015,
    MG_BLEND_OP_LIGHTEN = 1000148016,
    MG_BLEND_OP_COLORDODGE = 1000148017,
    MG_BLEND_OP_COLORBURN = 1000148018,
    MG_BLEND_OP_HARDLIGHT = 1000148019,
    MG_BLEND_OP_SOFTLIGHT = 1000148020,
    MG_BLEND_OP_DIFFERENCE = 1000148021,
    MG_BLEND_OP_EXCLUSION = 1000148022,
    MG_BLEND_OP_INVERT = 1000148023,
    MG_BLEND_OP_INVERT_RGB = 1000148024,
    MG_BLEND_OP_LINEARDODGE = 1000148025,
    MG_BLEND_OP_LINEARBURN = 1000148026,
    MG_BLEND_OP_VIVIDLIGHT = 1000148027,
    MG_BLEND_OP_LINEARLIGHT = 1000148028,
    MG_BLEND_OP_PINLIGHT = 1000148029,
    MG_BLEND_OP_HARDMIX = 1000148030,
    MG_BLEND_OP_HSL_HUE = 1000148031,
    MG_BLEND_OP_HSL_SATURATION = 1000148032,
    MG_BLEND_OP_HSL_COLOR = 1000148033,
    MG_BLEND_OP_HSL_LUMINOSITY = 1000148034,
    MG_BLEND_OP_PLUS = 1000148035,
    MG_BLEND_OP_PLUS_CLAMPED = 1000148036,
    MG_BLEND_OP_PLUS_CLAMPED_ALPHA = 1000148037,
    MG_BLEND_OP_PLUS_DARKER = 1000148038,
    MG_BLEND_OP_MINUS = 1000148039,
    MG_BLEND_OP_MINUS_CLAMPED = 1000148040,
    MG_BLEND_OP_CONTRAST = 1000148041,
    MG_BLEND_OP_INVERT_OVG = 1000148042,
    MG_BLEND_OP_RED = 1000148043,
    MG_BLEND_OP_GREEN = 1000148044,
    MG_BLEND_OP_BLUE = 1000148045
}
mg_blend_op;

typedef struct mg_multisample_state
{
    bool sample_shading_enable;
    bool alpha_to_coverage_enable;
    bool alpha_to_one_enable;
}
mg_multisample_state;

typedef struct mg_color_blend
{
    bool blend_enabled;

    mg_blend_factor src_color_blend_factor;
    mg_blend_factor dst_color_blend_factor;
    mg_blend_op color_blend_op;
    
    mg_blend_factor src_alpha_blend_factor;
    mg_blend_factor dst_alpha_blend_factor;
    mg_blend_op alpha_blend_op;
}
mg_color_blend;

typedef enum mg_compare_op
{
    MG_COMPARE_OP_NEVER = 0,
    MG_COMPARE_OP_LESS = 1,
    MG_COMPARE_OP_EQUAL = 2,
    MG_COMPARE_OP_LESS_OR_EQUAL = 3,
    MG_COMPARE_OP_GREATER = 4,
    MG_COMPARE_OP_NOT_EQUAL = 5,
    MG_COMPARE_OP_GREATER_OR_EQUAL = 6,
    MG_COMPARE_OP_ALWAYS = 7
}
mg_compare_op;

typedef struct mg_depth_stencil_state
{
    bool depth_test_enable;
    bool depth_write_enable;
    bool stencil_test_enable;

    mg_compare_op depth_compare_op;
}
mg_depth_stencil_state;

typedef enum mg_shader_stage
{
    MG_SHADER_STAGE_VERTEX = 0x00000001,
    MG_SHADER_STAGE_GEOMETRY = 0x00000008,
    MG_SHADER_STAGE_FRAGMENT = 0x00000010,
    MG_SHADER_STAGE_COMPUTE = 0x00000020
}
mg_shader_stage;

typedef enum mg_vertex_format
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
    MG_VERTEX_FORMAT_FLOAT4 = 109,

    MG_VERTEX_FORMAT_UBYTE4 = 41,
    MG_VERTEX_FORMAT_BYTE4 = 42,

    MG_VERTEX_FORMAT_UBYTE4N = 37,
    MG_VERTEX_FORMAT_BYTE4N = 38
}
mg_vertex_format;

typedef struct mg_vertex_attribute_info
{
    uint32_t location;
    uint32_t offset;
    mg_vertex_format format;
}
mg_vertex_attribute_info;

typedef struct mg_vertex_layout_info
{
    uint32_t stride;
    uint32_t attribute_count;
    mg_vertex_attribute_info *attributes;
}
mg_vertex_layout_info;

/*typedef enum mg_shader_type
{
	MG_SHADER_TYPE_VERTEX = 0x00000001,
	MG_SHADER_TYPE_GEOMETRY = 0x00000008,
	MG_SHADER_TYPE_FRAGMENT = 0x00000010,
	MG_SHADER_TYPE_COMPUTE = 0x00000020
}
mg_shader_type_t;*/

typedef struct mg_shader_source
{
	void *code;
	size_t size;
}
mg_shader_source;

typedef struct mg_shader
{
    mg_shader_source vertex;
    mg_shader_source fragment;
}
mg_shader;

typedef struct mg_pipeline_create_info
{
    mg_shader shader;
    mg_vertex_layout_info vertex_layout;

    mg_primitive_topology primitive_topology;
    mg_polygon_mode polygon_mode;
    mg_cull_mode cull_mode;
    mg_front_face front_face;
    mg_color_blend color_blend;
    mg_depth_stencil_state depth_stencil;

    mg_render_pass render_pass;
}
mg_pipeline_create_info;

typedef void *mg_pipeline;

typedef enum mg_present_mode
{
    MG_PRESENT_MODE_IMMEDIATE = 0,
    MG_PRESENT_MODE_MAILBOX = 1,
    MG_PRESENT_MODE_FIFO = 2,
    MG_PRESENT_MODE_FIFO_RELAXED = 3
}
mg_present_mode;

typedef struct mg_swapchain_config_info
{
    mg_pixel_format format;
    mg_present_mode present_mode;
    uint32_t width, height;
}
mg_swapchain_config_info;

typedef struct mgfx_init_info
{
    mg_platform *platform;
    mg_renderer_type type;
    mg_swapchain_config_info *swapchain_config_info;
}
mgfx_init_info;

MG_API inline void                     mgfx_initialize                      (mgfx_init_info *init_info);
MG_API inline void                     mgfx_shutdown                        (void);

MG_API inline void                     mgfx_begin                     		(void);
MG_API inline void                     mgfx_end                       		(void);

MG_API inline void                     mgfx_wait                            (void);

MG_API inline void                     mgfx_viewport                        (int32_t x, int32_t y, uint32_t width, uint32_t height);
MG_API inline void                     mgfx_scissor                         (int32_t x, int32_t y, uint32_t width, uint32_t height);

MG_API inline void                      mgfx_draw                           (uint32_t vertex_count, uint32_t first_vertex);
MG_API inline void                      mgfx_draw_indexed                   (uint32_t index_count, uint32_t first_index, uint32_t vertex_offset);

MG_API inline void                      mgfx_configure_swapchain            (mg_swapchain_config_info *config_info);

MG_API inline mg_render_pass            mgfx_create_render_pass             (mg_render_pass_create_info *create_info);
MG_API inline void                      mgfx_destroy_render_pass            (mg_render_pass render_pass);
MG_API inline void                      mgfx_begin_render_pass              (mg_render_pass render_pass, mg_framebuffer framebuffer, mg_render_pass_begin_info *begin_info);
MG_API inline void                      mgfx_begin_default_render_pass      (mg_render_pass_begin_info *begin_info);
MG_API inline void                      mgfx_end_render_pass                (void);

MG_API inline mg_pipeline               mgfx_create_pipeline                (mg_pipeline_create_info *create_info);
MG_API inline void                      mgfx_destroy_pipeline               (mg_pipeline pipeline);
MG_API inline void                      mgfx_bind_pipeline                  (mg_pipeline pipeline);

MG_API inline mg_vertex_buffer          mgfx_create_vertex_buffer           (size_t size, void *data);
MG_API inline void                      mgfx_destroy_vertex_buffer          (mg_vertex_buffer buffer);

MG_API inline mg_index_buffer           mgfx_create_index_buffer            (size_t size, void *data);
MG_API inline void                      mgfx_destroy_index_buffer           (mg_index_buffer buffer);

MG_API inline mg_dynamic_vertex_buffer  mgfx_create_dynamic_vertex_buffer   (size_t size);
MG_API inline void                      mgfx_destroy_dynamic_vertex_buffer  (mg_dynamic_vertex_buffer buffer);
MG_API inline void                      mgfx_update_dynamic_vertex_buffer   (mg_dynamic_vertex_buffer buffer, size_t size, void *data);

MG_API inline mg_dynamic_index_buffer   mgfx_create_dynamic_index_buffer    (size_t size);
MG_API inline void                      mgfx_destroy_dynamic_index_buffer   (mg_dynamic_index_buffer buffer);
MG_API inline void                      mgfx_update_dynamic_index_buffer    (mg_dynamic_index_buffer buffer, size_t size, void *data);

MG_API inline void                      mgfx_bind_vertex_buffer             (mg_vertex_buffer buffer);
MG_API inline void                      mgfx_bind_dynamic_vertex_buffer     (mg_dynamic_vertex_buffer buffer);
MG_API inline void                      mgfx_bind_index_buffer              (mg_index_buffer buffer, mg_index_type index_type);
MG_API inline void                      mgfx_bind_dynamic_index_buffer      (mg_index_buffer buffer, mg_index_type index_type);

MG_API inline void                      mgfx_bind_uniforms                  (uint32_t binding, size_t size, void *data);

MG_API inline mg_image                  mgfx_create_image                   (mg_image_create_info *create_info);
MG_API inline void                      mgfx_destroy_image                  (mg_image image);
MG_API inline void                      mgfx_update_image                   (mg_image image, mg_image_write_info *write_info);

MG_API inline mg_image_array            mgfx_create_image_array             (void);
MG_API inline void                      mgfx_destroy_image_array            (mg_image_array array);
MG_API inline void                      mgfx_update_image_array             (mg_image_array array, mg_image *images, mg_sampler *samplers, uint32_t count);
MG_API inline void                      mgfx_bind_image_array               (mg_image_array array);

MG_API inline mg_sampler                mgfx_create_sampler                 (mg_sampler_create_info *create_info);
MG_API inline void                      mgfx_destroy_sampler                (mg_sampler sampler);

MG_API inline mg_framebuffer            mgfx_create_framebuffer             (mg_framebuffer_create_info *create_info);
MG_API inline void                      mgfx_destroy_framebuffer            (mg_framebuffer framebuffer);

MG_API inline mg_renderer_type          mgfx_get_type                       (void);

#ifdef __cplusplus
}
#endif