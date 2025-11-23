#include "opengl_image.h"
#include "opengl_renderer.h"

#include <stdlib.h>
#include <assert.h>

static GLenum mg_opengl_get_internal_format(mg_pixel_format format)
{
    switch (format)
    {
        case MG_PIXEL_FORMAT_R8_UNORM:
        case MG_PIXEL_FORMAT_R8_SNORM:
        case MG_PIXEL_FORMAT_R8_USCALED:
        case MG_PIXEL_FORMAT_R8_SSCALED:
        case MG_PIXEL_FORMAT_R8_UINT:
        case MG_PIXEL_FORMAT_R8_SINT:
        case MG_PIXEL_FORMAT_R8_SRGB:
            return GL_R8;

        case MG_PIXEL_FORMAT_R32_UINT:
            return GL_R32UI;
        case MG_PIXEL_FORMAT_R32_SINT:
            return GL_R32I;

        case MG_PIXEL_FORMAT_RG8_UNORM:
        case MG_PIXEL_FORMAT_RG8_SNORM:
        case MG_PIXEL_FORMAT_RG8_USCALED:
        case MG_PIXEL_FORMAT_RG8_SSCALED:
        case MG_PIXEL_FORMAT_RG8_UINT:
        case MG_PIXEL_FORMAT_RG8_SINT:
        case MG_PIXEL_FORMAT_RG8_SRGB:
            return GL_RG8;

        case MG_PIXEL_FORMAT_RG32_UINT:
            return GL_RG32UI;
        case MG_PIXEL_FORMAT_RG32_SINT:
            return GL_RG32I;

        case MG_PIXEL_FORMAT_RGB8_UNORM:
        case MG_PIXEL_FORMAT_RGB8_SNORM:
        case MG_PIXEL_FORMAT_RGB8_USCALED:
        case MG_PIXEL_FORMAT_RGB8_SSCALED:
        case MG_PIXEL_FORMAT_RGB8_UINT:
        case MG_PIXEL_FORMAT_RGB8_SINT:

        case MG_PIXEL_FORMAT_BGR8_UNORM:
        case MG_PIXEL_FORMAT_BGR8_SNORM:
        case MG_PIXEL_FORMAT_BGR8_USCALED:
        case MG_PIXEL_FORMAT_BGR8_SSCALED:
        case MG_PIXEL_FORMAT_BGR8_UINT:
        case MG_PIXEL_FORMAT_BGR8_SINT:
            return GL_RGB8;

        case MG_PIXEL_FORMAT_RGB8_SRGB:
        case MG_PIXEL_FORMAT_BGR8_SRGB:
            return GL_SRGB8;

        case MG_PIXEL_FORMAT_RGB32_UINT:
            return GL_RGB32UI;
        case MG_PIXEL_FORMAT_RGB32_SINT:
            return GL_RGB32I;

        case MG_PIXEL_FORMAT_RGBA8_UNORM:
        case MG_PIXEL_FORMAT_RGBA8_SNORM:
        case MG_PIXEL_FORMAT_RGBA8_USCALED:
        case MG_PIXEL_FORMAT_RGBA8_SSCALED:
        case MG_PIXEL_FORMAT_RGBA8_UINT:
        case MG_PIXEL_FORMAT_RGBA8_SINT:

        case MG_PIXEL_FORMAT_BGRA8_UNORM:
        case MG_PIXEL_FORMAT_BGRA8_SNORM:
        case MG_PIXEL_FORMAT_BGRA8_USCALED:
        case MG_PIXEL_FORMAT_BGRA8_SSCALED:
        case MG_PIXEL_FORMAT_BGRA8_UINT:
        case MG_PIXEL_FORMAT_BGRA8_SINT:
            return GL_RGBA8;

        case MG_PIXEL_FORMAT_RGBA8_SRGB:
        case MG_PIXEL_FORMAT_BGRA8_SRGB:
            return GL_SRGB8_ALPHA8;

        case MG_PIXEL_FORMAT_RGBA32_UINT:
            return GL_RGBA32UI;
        case MG_PIXEL_FORMAT_RGBA32_SINT:
            return GL_RGBA32I;

#if !MG_PLATFORM_EMSCRIPTEN
        case MG_PIXEL_FORMAT_R16_UNORM:
        case MG_PIXEL_FORMAT_R16_SNORM:
        case MG_PIXEL_FORMAT_R16_USCALED:
        case MG_PIXEL_FORMAT_R16_SSCALED:
        case MG_PIXEL_FORMAT_R16_UINT:
        case MG_PIXEL_FORMAT_R16_SINT:
            return GL_R16;

        case MG_PIXEL_FORMAT_RG16_UNORM:
        case MG_PIXEL_FORMAT_RG16_SNORM:
        case MG_PIXEL_FORMAT_RG16_USCALED:
        case MG_PIXEL_FORMAT_RG16_SSCALED:
        case MG_PIXEL_FORMAT_RG16_UINT:
        case MG_PIXEL_FORMAT_RG16_SINT:
            return GL_RG16;

        case MG_PIXEL_FORMAT_RGB16_UNORM:
        case MG_PIXEL_FORMAT_RGB16_SNORM:
        case MG_PIXEL_FORMAT_RGB16_USCALED:
        case MG_PIXEL_FORMAT_RGB16_SSCALED:
        case MG_PIXEL_FORMAT_RGB16_UINT:
        case MG_PIXEL_FORMAT_RGB16_SINT:
            return GL_RGB16;

        case MG_PIXEL_FORMAT_RGBA16_UNORM:
        case MG_PIXEL_FORMAT_RGBA16_SNORM:
        case MG_PIXEL_FORMAT_RGBA16_USCALED:
        case MG_PIXEL_FORMAT_RGBA16_SSCALED:
        case MG_PIXEL_FORMAT_RGBA16_UINT:
        case MG_PIXEL_FORMAT_RGBA16_SINT:
            return GL_RGBA16;
#endif

        case MG_PIXEL_FORMAT_D16_UNORM_S8_UINT:
        case MG_PIXEL_FORMAT_D24_UNORM_S8_UINT:
            return GL_DEPTH24_STENCIL8;

        case MG_PIXEL_FORMAT_D32_SFLOAT:
        case MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT:
            return GL_DEPTH32F_STENCIL8;
    }
    return GL_RGBA8;
}

static GLenum mg_opengl_get_format(mg_pixel_format format)
{
    switch (format)
    {
        case MG_PIXEL_FORMAT_R8_UNORM:
        case MG_PIXEL_FORMAT_R8_SNORM:
        case MG_PIXEL_FORMAT_R8_USCALED:
        case MG_PIXEL_FORMAT_R8_SSCALED:
        case MG_PIXEL_FORMAT_R8_UINT:
        case MG_PIXEL_FORMAT_R8_SINT:
        case MG_PIXEL_FORMAT_R8_SRGB:
            return GL_RED;

        case MG_PIXEL_FORMAT_RG8_UNORM:
        case MG_PIXEL_FORMAT_RG8_SNORM:
        case MG_PIXEL_FORMAT_RG8_USCALED:
        case MG_PIXEL_FORMAT_RG8_SSCALED:
        case MG_PIXEL_FORMAT_RG8_UINT:
        case MG_PIXEL_FORMAT_RG8_SINT:
        case MG_PIXEL_FORMAT_RG8_SRGB:
            return GL_RG;

        case MG_PIXEL_FORMAT_RGB8_UNORM:
        case MG_PIXEL_FORMAT_RGB8_SNORM:
        case MG_PIXEL_FORMAT_RGB8_USCALED:
        case MG_PIXEL_FORMAT_RGB8_SSCALED:
        case MG_PIXEL_FORMAT_RGB8_UINT:
        case MG_PIXEL_FORMAT_RGB8_SINT:
        case MG_PIXEL_FORMAT_RGB8_SRGB:
            return GL_RGB;

        case MG_PIXEL_FORMAT_RGBA8_UNORM:
        case MG_PIXEL_FORMAT_RGBA8_SNORM:
        case MG_PIXEL_FORMAT_RGBA8_USCALED:
        case MG_PIXEL_FORMAT_RGBA8_SSCALED:
        case MG_PIXEL_FORMAT_RGBA8_UINT:
        case MG_PIXEL_FORMAT_RGBA8_SINT:
        case MG_PIXEL_FORMAT_RGBA8_SRGB:
            return GL_RGBA;

#if !MG_PLATFORM_EMSCRIPTEN
        case MG_PIXEL_FORMAT_BGR8_UNORM:
        case MG_PIXEL_FORMAT_BGR8_SNORM:
        case MG_PIXEL_FORMAT_BGR8_USCALED:
        case MG_PIXEL_FORMAT_BGR8_SSCALED:
        case MG_PIXEL_FORMAT_BGR8_UINT:
        case MG_PIXEL_FORMAT_BGR8_SINT:
        case MG_PIXEL_FORMAT_BGR8_SRGB:
            return GL_BGR;

        case MG_PIXEL_FORMAT_BGRA8_UNORM:
        case MG_PIXEL_FORMAT_BGRA8_SNORM:
        case MG_PIXEL_FORMAT_BGRA8_USCALED:
        case MG_PIXEL_FORMAT_BGRA8_SSCALED:
        case MG_PIXEL_FORMAT_BGRA8_UINT:
        case MG_PIXEL_FORMAT_BGRA8_SINT:
        case MG_PIXEL_FORMAT_BGRA8_SRGB:
            return GL_BGRA;
#endif
        case MG_PIXEL_FORMAT_D32_SFLOAT:
        case MG_PIXEL_FORMAT_D16_UNORM_S8_UINT:
        case MG_PIXEL_FORMAT_D24_UNORM_S8_UINT:
        case MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT:
            return GL_DEPTH_STENCIL;
    }
    return GL_RGBA;
}

static GLint mg_opengl_get_filter(mg_sampler_filter filter)
{
    switch (filter)
    {
        case MG_SAMPLER_FILTER_NEAREST: return GL_NEAREST;
        case MG_SAMPLER_FILTER_LINEAR: return GL_LINEAR;
    }
    return GL_NEAREST;
}

static GLint mg_opengl_get_address_mode(mg_sampler_address_mode address_mode)
{
    switch (address_mode)
    {
    case MG_SAMPLER_ADDRESS_MODE_REPEAT:
        return GL_REPEAT;
    case MG_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
        return GL_MIRRORED_REPEAT;
#if !MG_PLATFORM_EMSCRIPTEN
    case MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
    case MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
        return GL_CLAMP_TO_BORDER;
    case MG_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
        return GL_MIRROR_CLAMP_TO_EDGE;
    default:
        return GL_REPEAT;
#else
    default:
        return GL_CLAMP_TO_EDGE;
#endif
    }
}

mg_opengl_image *mg_opengl_create_image(const mg_image_create_info *create_info)
{
    mg_opengl_image *image = (mg_opengl_image*)malloc(sizeof(mg_opengl_image));

    glGenTextures(1, &image->texture_id);

    switch (create_info->type)
    {
    case MG_IMAGE_TYPE_2D: image->texture_target = GL_TEXTURE_2D; break;
    case MG_IMAGE_TYPE_3D: image->texture_target = GL_TEXTURE_3D; break;
    case MG_IMAGE_TYPE_CUBE: image->texture_target = GL_TEXTURE_CUBE_MAP; break;
    }

    glBindTexture(image->texture_target, image->texture_id);

    image->internal_format = mg_opengl_get_internal_format(create_info->format);
    image->format = mg_opengl_get_format(create_info->format);

    const GLuint usage = create_info->usage == MG_IMAGE_USAGE_COLOR_ATTACHMENT ? GL_UNSIGNED_BYTE : GL_UNSIGNED_INT_24_8;
    glTexImage2D(image->texture_target, 0, image->internal_format,
        create_info->width, create_info->height, 0, image->format, usage, NULL);

    return image;
}

void mg_opengl_destroy_image(mg_opengl_image *image)
{
    glDeleteTextures(1, &image->texture_id);
    free(image);
}

void mg_opengl_update_image(mg_opengl_image *image, const mg_image_update_info *update_info)
{
	glBindTexture(image->texture_target, image->texture_id);
    if (image->texture_target == GL_TEXTURE_2D)
        glTexImage2D(GL_TEXTURE_2D, 0, image->internal_format,
            update_info->width, update_info->height, 0, image->format, GL_UNSIGNED_BYTE, update_info->data);
    else if (image->texture_target == GL_TEXTURE_3D)
        glTexImage3D(GL_TEXTURE_3D, 0, image->internal_format,
            update_info->width, update_info->height, update_info->depth, 0, image->format, GL_UNSIGNED_BYTE, update_info->data);
}

void mg_opengl_bind_image(mg_opengl_image *image, mg_opengl_sampler *sampler, uint32_t binding)
{
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(image->texture_target, image->texture_id);
    glTexParameteri(image->texture_target, GL_TEXTURE_MIN_FILTER, sampler->min_filter);
    glTexParameteri(image->texture_target, GL_TEXTURE_MAG_FILTER, sampler->mag_filter);
    glTexParameteri(image->texture_target, GL_TEXTURE_WRAP_S, sampler->address_mode_u);
    glTexParameteri(image->texture_target, GL_TEXTURE_WRAP_T, sampler->address_mode_v);
    glTexParameteri(image->texture_target, GL_TEXTURE_WRAP_R, sampler->address_mode_w);
}

mg_opengl_sampler *mg_opengl_create_sampler(const mg_sampler_create_info *create_info)
{
    mg_opengl_sampler *sampler = (mg_opengl_sampler*)malloc(sizeof(mg_opengl_sampler));

    sampler->min_filter = mg_opengl_get_filter(create_info->min_filter);
    sampler->mag_filter = mg_opengl_get_filter(create_info->mag_filter);

    sampler->address_mode_u = mg_opengl_get_address_mode(create_info->address_mode_u);
    sampler->address_mode_v = mg_opengl_get_address_mode(create_info->address_mode_v);
    sampler->address_mode_w = mg_opengl_get_address_mode(create_info->address_mode_w);

    return sampler;
}

void mg_opengl_destroy_sampler(mg_opengl_sampler *sampler)
{
    free(sampler);
}