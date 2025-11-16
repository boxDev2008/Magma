#include "vulkan_descriptor_cache.h"
#include "vulkan_renderer.h"

#include <string.h>
#include <stdlib.h>

static uint64_t mg_vulkan_hash_descriptor_key(const mg_vulkan_descriptor_set_key* key)
{
    uint64_t hash = 14695981039346656037ULL;
    const uint8_t* data = (const uint8_t*)key->bindings;
    size_t size = sizeof(mg_vulkan_descriptor_binding) * key->binding_count;
    
    for (size_t i = 0; i < size; i++)
    {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    
    return hash;
}

static bool mg_vulkan_compare_descriptor_keys(const mg_vulkan_descriptor_set_key* a, const mg_vulkan_descriptor_set_key* b)
{
    if (a->binding_count != b->binding_count || a->hash != b->hash)
        return false;
    
    return memcmp(a->bindings, b->bindings, sizeof(mg_vulkan_descriptor_binding) * a->binding_count) == 0;
}

static int32_t mg_vulkan_find_cached_set(mg_vulkan_descriptor_cache* cache, const mg_vulkan_descriptor_set_key* key)
{
    for (uint32_t i = 0; i < cache->cache_size; i++)
        if (mg_vulkan_compare_descriptor_keys(&cache->cache[i].key, key))
            return (int32_t)i;
    return -1;
}

VkDescriptorSet mg_vulkan_commit_image_descriptor_cache(mg_vulkan_descriptor_cache* cache)
{
    if (cache->pending_image_binding_count == 0)
        return VK_NULL_HANDLE;

    mg_vulkan_descriptor_set_key key;
    memcpy(key.bindings, cache->pending_image_bindings, sizeof(mg_vulkan_descriptor_binding) * cache->pending_image_binding_count);
    key.binding_count = cache->pending_image_binding_count;
    key.hash = mg_vulkan_hash_descriptor_key(&key);

    int32_t cached_index = mg_vulkan_find_cached_set(cache, &key);
    if (cached_index >= 0)
    {
        cache->pending_image_binding_count = 0;
        return cache->sets[cached_index];
    }

    if (cache->cache_size >= MG_CONFIG_MAX_DESCRIPTOR_CACHE)
    {
        cache->pending_image_binding_count = 0;
        return VK_NULL_HANDLE;
    }

    uint32_t new_index = cache->cache_size;
    cache->cache_size++;
    
    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vk_ctx.descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vk_ctx.layouts.image_sampler_layout
    };
    
    vkAllocateDescriptorSets(vk_ctx.device.handle, &alloc_info, &cache->sets[new_index]);
    VkDescriptorSet descriptor_set = cache->sets[new_index];
    cache->cache[new_index].key = key;
    
    VkWriteDescriptorSet writes[MG_CONFIG_MAX_BINDABLE_IMAGES];
    VkDescriptorImageInfo image_infos[MG_CONFIG_MAX_BINDABLE_IMAGES];
    
    for (uint32_t i = 0; i < cache->pending_image_binding_count; i++)
    {
        image_infos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_infos[i].imageView = cache->pending_image_bindings[i].image_view;
        image_infos[i].sampler = cache->pending_image_bindings[i].sampler;
        
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].pNext = NULL;
        writes[i].dstSet = descriptor_set;
        writes[i].dstBinding = cache->pending_image_bindings[i].binding;
        writes[i].dstArrayElement = 0;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[i].descriptorCount = 1;
        writes[i].pImageInfo = &image_infos[i];
        writes[i].pBufferInfo = NULL;
        writes[i].pTexelBufferView = NULL;
    }

    vkUpdateDescriptorSets(vk_ctx.device.handle, cache->pending_image_binding_count, writes, 0, NULL);
   
    cache->pending_image_binding_count = 0;

    return descriptor_set;
}

static void mg_vulkan_remove_cache_entry(mg_vulkan_descriptor_cache* cache, uint32_t index)
{
    if (cache->cache_size == 0 || index >= cache->cache_size)
        return;
    
    vkFreeDescriptorSets(vk_ctx.device.handle, vk_ctx.descriptor_pool, 1, &cache->sets[index]);
    
    uint32_t last_index = cache->cache_size - 1;
    if (index != last_index)
    {
        cache->cache[index] = cache->cache[last_index];
        cache->sets[index] = cache->sets[last_index];
    }
    
    cache->cache_size--;
}

void mg_vulkan_descriptor_cache_invalidate_image_view(mg_vulkan_descriptor_cache* cache, VkImageView imag_view)
{
    for (uint32_t i = 0; i < cache->cache_size; i++)
        for (uint32_t j = 0; j < cache->cache[i].key.binding_count; j++)
            if (cache->cache[i].key.bindings[j].image_view == imag_view)
            {
                mg_vulkan_remove_cache_entry(cache, i);
                i--;
                break;
            }
}

void mg_vulkan_descriptor_cache_invalidate_sampler(mg_vulkan_descriptor_cache* cache, VkSampler sampler)
{
    for (uint32_t i = 0; i < cache->cache_size; i++)
        for (uint32_t j = 0; j < cache->cache[i].key.binding_count; j++)
            if (cache->cache[i].key.bindings[j].sampler == sampler)
            {
                mg_vulkan_remove_cache_entry(cache, i);
                i--;
                break;
            }
}