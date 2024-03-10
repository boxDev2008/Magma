#include "opengl_image.h"

#include <stdlib.h>

static GLenum mg_opengl_get_internal_format(mg_pixel_format_t format)
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

        case MG_PIXEL_FORMAT_R16_UNORM:
        case MG_PIXEL_FORMAT_R16_SNORM:
        case MG_PIXEL_FORMAT_R16_USCALED:
        case MG_PIXEL_FORMAT_R16_SSCALED:
        case MG_PIXEL_FORMAT_R16_UINT:
        case MG_PIXEL_FORMAT_R16_SINT:
            return GL_R16;

        case MG_PIXEL_FORMAT_R32_UINT:
            return GL_R32UI;
        case MG_PIXEL_FORMAT_R32_SINT:
            return GL_R32I;

        case MG_PIXEL_FORMAT_R8G8_UNORM:
        case MG_PIXEL_FORMAT_R8G8_SNORM:
        case MG_PIXEL_FORMAT_R8G8_USCALED:
        case MG_PIXEL_FORMAT_R8G8_SSCALED:
        case MG_PIXEL_FORMAT_R8G8_UINT:
        case MG_PIXEL_FORMAT_R8G8_SINT:
        case MG_PIXEL_FORMAT_R8G8_SRGB:
            return GL_RG8;

        case MG_PIXEL_FORMAT_R16G16_UNORM:
        case MG_PIXEL_FORMAT_R16G16_SNORM:
        case MG_PIXEL_FORMAT_R16G16_USCALED:
        case MG_PIXEL_FORMAT_R16G16_SSCALED:
        case MG_PIXEL_FORMAT_R16G16_UINT:
        case MG_PIXEL_FORMAT_R16G16_SINT:
            return GL_RG16;

        case MG_PIXEL_FORMAT_R32G32_UINT:
            return GL_RG32UI;
        case MG_PIXEL_FORMAT_R32G32_SINT:
            return GL_RG32I;

        case MG_PIXEL_FORMAT_R8G8B8_UNORM:
        case MG_PIXEL_FORMAT_R8G8B8_SNORM:
        case MG_PIXEL_FORMAT_R8G8B8_USCALED:
        case MG_PIXEL_FORMAT_R8G8B8_SSCALED:
        case MG_PIXEL_FORMAT_R8G8B8_UINT:
        case MG_PIXEL_FORMAT_R8G8B8_SINT:

        case MG_PIXEL_FORMAT_B8G8R8_UNORM:
        case MG_PIXEL_FORMAT_B8G8R8_SNORM:
        case MG_PIXEL_FORMAT_B8G8R8_USCALED:
        case MG_PIXEL_FORMAT_B8G8R8_SSCALED:
        case MG_PIXEL_FORMAT_B8G8R8_UINT:
        case MG_PIXEL_FORMAT_B8G8R8_SINT:
            return GL_RGB8;

        case MG_PIXEL_FORMAT_R8G8B8_SRGB:
        case MG_PIXEL_FORMAT_B8G8R8_SRGB:
            return GL_SRGB8;

        case MG_PIXEL_FORMAT_R16G16B16_UNORM:
        case MG_PIXEL_FORMAT_R16G16B16_SNORM:
        case MG_PIXEL_FORMAT_R16G16B16_USCALED:
        case MG_PIXEL_FORMAT_R16G16B16_SSCALED:
        case MG_PIXEL_FORMAT_R16G16B16_UINT:
        case MG_PIXEL_FORMAT_R16G16B16_SINT:
            return GL_RGB16;

        case MG_PIXEL_FORMAT_R32G32B32_UINT:
            return GL_RGB32UI;
        case MG_PIXEL_FORMAT_R32G32B32_SINT:
            return GL_RGB32I;

        case MG_PIXEL_FORMAT_R8G8B8A8_UNORM:
        case MG_PIXEL_FORMAT_R8G8B8A8_SNORM:
        case MG_PIXEL_FORMAT_R8G8B8A8_USCALED:
        case MG_PIXEL_FORMAT_R8G8B8A8_SSCALED:
        case MG_PIXEL_FORMAT_R8G8B8A8_UINT:
        case MG_PIXEL_FORMAT_R8G8B8A8_SINT:

        case MG_PIXEL_FORMAT_B8G8R8A8_UNORM:
        case MG_PIXEL_FORMAT_B8G8R8A8_SNORM:
        case MG_PIXEL_FORMAT_B8G8R8A8_USCALED:
        case MG_PIXEL_FORMAT_B8G8R8A8_SSCALED:
        case MG_PIXEL_FORMAT_B8G8R8A8_UINT:
        case MG_PIXEL_FORMAT_B8G8R8A8_SINT:
            return GL_RGBA8;

        case MG_PIXEL_FORMAT_R8G8B8A8_SRGB:
        case MG_PIXEL_FORMAT_B8G8R8A8_SRGB:
            return GL_SRGB8_ALPHA8;

        case MG_PIXEL_FORMAT_R16G16B16A16_UNORM:
        case MG_PIXEL_FORMAT_R16G16B16A16_SNORM:
        case MG_PIXEL_FORMAT_R16G16B16A16_USCALED:
        case MG_PIXEL_FORMAT_R16G16B16A16_SSCALED:
        case MG_PIXEL_FORMAT_R16G16B16A16_UINT:
        case MG_PIXEL_FORMAT_R16G16B16A16_SINT:
            return GL_RGBA16;

        case MG_PIXEL_FORMAT_R32G32B32A32_UINT:
            return GL_RGBA32UI;
        case MG_PIXEL_FORMAT_R32G32B32A32_SINT:
            return GL_RGBA32I;

        case MG_PIXEL_FORMAT_D32_SFLOAT:
            return GL_DEPTH_STENCIL;

        case MG_PIXEL_FORMAT_D16_UNORM_S8_UINT:
            return GL_DEPTH24_STENCIL8;

        case MG_PIXEL_FORMAT_D24_UNORM_S8_UINT:
            return GL_DEPTH32F_STENCIL8;

        case MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT:
            return GL_DEPTH32F_STENCIL8;
    }
}

static GLenum mg_opengl_get_format(mg_pixel_format_t format)
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

        case MG_PIXEL_FORMAT_R8G8_UNORM:
        case MG_PIXEL_FORMAT_R8G8_SNORM:
        case MG_PIXEL_FORMAT_R8G8_USCALED:
        case MG_PIXEL_FORMAT_R8G8_SSCALED:
        case MG_PIXEL_FORMAT_R8G8_UINT:
        case MG_PIXEL_FORMAT_R8G8_SINT:
        case MG_PIXEL_FORMAT_R8G8_SRGB:
            return GL_RG;

        case MG_PIXEL_FORMAT_R8G8B8_UNORM:
        case MG_PIXEL_FORMAT_R8G8B8_SNORM:
        case MG_PIXEL_FORMAT_R8G8B8_USCALED:
        case MG_PIXEL_FORMAT_R8G8B8_SSCALED:
        case MG_PIXEL_FORMAT_R8G8B8_UINT:
        case MG_PIXEL_FORMAT_R8G8B8_SINT:
        case MG_PIXEL_FORMAT_R8G8B8_SRGB:
            return GL_RGB;

        case MG_PIXEL_FORMAT_B8G8R8_UNORM:
        case MG_PIXEL_FORMAT_B8G8R8_SNORM:
        case MG_PIXEL_FORMAT_B8G8R8_USCALED:
        case MG_PIXEL_FORMAT_B8G8R8_SSCALED:
        case MG_PIXEL_FORMAT_B8G8R8_UINT:
        case MG_PIXEL_FORMAT_B8G8R8_SINT:
        case MG_PIXEL_FORMAT_B8G8R8_SRGB:
            return GL_BGR;

        case MG_PIXEL_FORMAT_R8G8B8A8_UNORM:
        case MG_PIXEL_FORMAT_R8G8B8A8_SNORM:
        case MG_PIXEL_FORMAT_R8G8B8A8_USCALED:
        case MG_PIXEL_FORMAT_R8G8B8A8_SSCALED:
        case MG_PIXEL_FORMAT_R8G8B8A8_UINT:
        case MG_PIXEL_FORMAT_R8G8B8A8_SINT:
        case MG_PIXEL_FORMAT_R8G8B8A8_SRGB:
            return GL_RGBA;

        case MG_PIXEL_FORMAT_B8G8R8A8_UNORM:
        case MG_PIXEL_FORMAT_B8G8R8A8_SNORM:
        case MG_PIXEL_FORMAT_B8G8R8A8_USCALED:
        case MG_PIXEL_FORMAT_B8G8R8A8_SSCALED:
        case MG_PIXEL_FORMAT_B8G8R8A8_UINT:
        case MG_PIXEL_FORMAT_B8G8R8A8_SINT:
        case MG_PIXEL_FORMAT_B8G8R8A8_SRGB:
            return GL_BGRA;

        case MG_PIXEL_FORMAT_D32_SFLOAT:
        case MG_PIXEL_FORMAT_D16_UNORM_S8_UINT:
        case MG_PIXEL_FORMAT_D24_UNORM_S8_UINT:
        case MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT:
            return GL_DEPTH_STENCIL;
    }
}

static GLint mg_opengl_get_filter(mg_sampler_filter_t filter)
{
    switch (filter)
    {
        case MG_SAMPLER_FILTER_NEAREST: return GL_NEAREST;
        case MG_SAMPLER_FILTER_LINEAR: return GL_LINEAR;
    }
}

static GLint mg_opengl_get_address_mode(mg_sampler_address_mode_t address_mode)
{
    switch (address_mode)
    {
    case MG_SAMPLER_ADDRESS_MODE_REPEAT:
        return GL_REPEAT;
    case MG_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
        return GL_MIRRORED_REPEAT;
    case MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
    case MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
        return GL_CLAMP_TO_BORDER;
    case MG_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
        return GL_MIRROR_CLAMP_TO_EDGE;
    }
}

mg_opengl_image_t *mg_opengl_create_image(mg_image_create_info_t *create_info)
{
    mg_opengl_image_t *image = (mg_opengl_image_t *)malloc(sizeof(mg_opengl_image_t));

    glGenTextures(1, &image->texture_id);

    switch (create_info->type)
    {
    case MG_IMAGE_TYPE_1D: image->texture_target = GL_TEXTURE_1D;
        break;
    case MG_IMAGE_TYPE_2D: image->texture_target = GL_TEXTURE_2D;
        break;
    case MG_IMAGE_TYPE_3D: image->texture_target = GL_TEXTURE_3D;
        break;
    case MG_IMAGE_TYPE_CUBE: image->texture_target = GL_TEXTURE_3D;
        break;
    }

    glBindTexture(image->texture_target, image->texture_id);

    if (create_info->usage == MG_IMAGE_USAGE_COLOR_ATTACHMENT)
        glTexImage2D(image->texture_target, 0, mg_opengl_get_internal_format(create_info->format),
            create_info->width, create_info->height, 0, mg_opengl_get_format(create_info->format), GL_UNSIGNED_BYTE, NULL);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, mg_opengl_get_internal_format(create_info->format),
            create_info->width, create_info->height, 0, mg_opengl_get_format(create_info->format), GL_UNSIGNED_INT_24_8, NULL);

    return image;
}

void mg_opengl_destroy_image(mg_opengl_image_t *image)
{
    glDeleteTextures(1, &image->texture_id);
    free(image);
}

void mg_opengl_write_image(mg_opengl_image_t *image, mg_image_write_info_t *write_info)
{
	glBindTexture(image->texture_target, image->texture_id);
	glTexImage2D(image->texture_target, 0, mg_opengl_get_internal_format(write_info->format),
        write_info->width, write_info->height, 0, mg_opengl_get_format(write_info->format), GL_UNSIGNED_BYTE, write_info->data);
}

void mg_opengl_update_image(mg_opengl_image_t *image, mg_opengl_sampler_t *sampler, uint32_t binding)
{
    glActiveTexture(GL_TEXTURE0 + binding);
	glBindTexture(image->texture_target, image->texture_id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler->address_mode_u);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler->address_mode_v);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, sampler->address_mode_w);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler->min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler->mag_filter);
    image->binding = binding;
}

void mg_opengl_bind_image(mg_opengl_image_t *image, mg_opengl_pipeline_t *pipeline)
{
    glActiveTexture(GL_TEXTURE0 + image->binding);
	glBindTexture(image->texture_target, image->texture_id);
}

mg_opengl_sampler_t *mg_opengl_create_sampler(mg_sampler_create_info_t *create_info)
{
    mg_opengl_sampler_t *sampler = (mg_opengl_sampler_t*)malloc(sizeof(mg_opengl_sampler_t));

    sampler->min_filter = mg_opengl_get_filter(create_info->min_filter);
    sampler->mag_filter = mg_opengl_get_filter(create_info->mag_filter);

    sampler->address_mode_u = mg_opengl_get_address_mode(create_info->address_mode_u);
    sampler->address_mode_v = mg_opengl_get_address_mode(create_info->address_mode_v);
    sampler->address_mode_w = mg_opengl_get_address_mode(create_info->address_mode_w);

    return sampler;
}

void mg_opengl_destroy_sampler(mg_opengl_sampler_t *sampler)
{
    free(sampler);
}

mg_opengl_framebuffer_t *mg_opengl_create_framebuffer(mg_framebuffer_create_info_t *create_info)
{
    mg_opengl_framebuffer_t *framebuffer = (mg_opengl_framebuffer_t*)malloc(sizeof(mg_opengl_framebuffer_t));

    glGenFramebuffers(1, &framebuffer->id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

    mg_opengl_image_t *color_attachment = (mg_opengl_image_t*)create_info->color_attachment.internal_data;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_attachment->texture_target, color_attachment->texture_id, 0);

    if (create_info->depth_stencil_attachment.internal_data)
    {
        mg_opengl_image_t *depth_stencil_attachment = (mg_opengl_image_t*)create_info->depth_stencil_attachment.internal_data;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, color_attachment->texture_target, depth_stencil_attachment->texture_id, 0);  
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return framebuffer;
}

void mg_opengl_destroy_framebuffer(mg_opengl_framebuffer_t *framebuffer)
{
    glDeleteFramebuffers(1, &framebuffer->id);
    free(framebuffer);
}