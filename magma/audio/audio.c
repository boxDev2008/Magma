#include "audio.h"

#include <stdlib.h>

typedef struct mg_audio_data
{
    ma_resource_manager resource_manager;
    ma_engine sfx_engine;
    ma_engine music_engine;
}
mg_audio_data_t;

static mg_audio_data_t audio_data;

void mg_audio_initialize(void)
{
    ma_resource_manager_config resource_manager_config = ma_resource_manager_config_init();
    resource_manager_config.decodedFormat = ma_format_f32;
    resource_manager_config.decodedChannels = 0;
    resource_manager_config.decodedSampleRate = 48000;

    ma_resource_manager_init(&resource_manager_config, &audio_data.resource_manager);

    ma_engine_config engine_config;
    engine_config = ma_engine_config_init();
    engine_config.pResourceManager = &audio_data.resource_manager;
    engine_config.noAutoStart = MA_FALSE;
   
    ma_engine_init(&engine_config, &audio_data.sfx_engine);
}

void mg_audio_shutdown(void)
{
    ma_engine_uninit(&audio_data.sfx_engine);
    ma_resource_manager_uninit(&audio_data.resource_manager);
}

mg_sound_resource *mg_audio_create_sound_resource_from_file(const char *file_name)
{
    mg_sound_resource *resource = (mg_sound_resource*)malloc(sizeof(mg_sound_resource));
    ma_sound_init_from_file(&audio_data.sfx_engine, file_name, MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, resource);
    return resource;
}

void mg_audio_destroy_sound_resource(mg_sound_resource *resource)
{
    ma_sound_uninit(resource);
    free(resource);
}

mg_sound *mg_audio_play_sound_internal(mg_sound_resource *resource)
{
    ma_result result = MA_SUCCESS;
    ma_sound_inlined* p_sound = NULL;
    ma_sound_inlined* p_next_sound = NULL;

    ma_node *p_node = ma_node_graph_get_endpoint(&audio_data.sfx_engine.nodeGraph);

    ma_spinlock_lock(&audio_data.sfx_engine.inlinedSoundLock);
    {
        ma_uint32 sound_flags = 0;

        for (p_next_sound = audio_data.sfx_engine.pInlinedSoundHead; p_next_sound != NULL; p_next_sound = p_next_sound->pNext)
        {
            if (ma_sound_at_end(&p_next_sound->sound))
            {
                p_sound = p_next_sound;
                break;
            }
        }

        if (p_sound != NULL)
        {
            if (audio_data.sfx_engine.pInlinedSoundHead == p_sound)
                audio_data.sfx_engine.pInlinedSoundHead =  p_sound->pNext;

            if (p_sound->pPrev != NULL)
                p_sound->pPrev->pNext = p_sound->pNext;

            if (p_sound->pNext != NULL)
                p_sound->pNext->pPrev = p_sound->pPrev;

            ma_sound_uninit(&p_next_sound->sound);
        }
        else
            p_sound = (ma_sound_inlined*)ma_malloc(sizeof(*p_sound), &audio_data.sfx_engine.allocationCallbacks);

        if (p_sound != NULL)
        {
            sound_flags |= MA_SOUND_FLAG_ASYNC;
            sound_flags |= MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT;
            //sound_flags |= MA_SOUND_FLAG_NO_PITCH;
            sound_flags |= MA_SOUND_FLAG_NO_SPATIALIZATION;

            result = ma_sound_init_copy(&audio_data.sfx_engine, resource, sound_flags, NULL, &p_sound->sound);
            if (result == MA_SUCCESS)
            {
                result = ma_node_attach_output_bus(p_sound, 0, p_node, 0);
                if (result == MA_SUCCESS)
                {
                    p_sound->pNext = audio_data.sfx_engine.pInlinedSoundHead;
                    p_sound->pPrev = NULL;

                    audio_data.sfx_engine.pInlinedSoundHead = p_sound;
                    if (p_sound->pNext != NULL)
                    {
                        p_sound->pNext->pPrev = p_sound;
                    }
                }
                else
                    ma_free(p_sound, &audio_data.sfx_engine.allocationCallbacks);
            }
            else
                ma_free(p_sound, &audio_data.sfx_engine.allocationCallbacks);
        }
        else
            result = MA_OUT_OF_MEMORY;
    }
    ma_spinlock_unlock(&audio_data.sfx_engine.inlinedSoundLock);

    result = ma_sound_start(&p_sound->sound);
    
    return &p_sound->sound;
}

void mg_audio_play_sound(mg_sound_resource *resource, float volume, float pitch)
{
    mg_sound *sound = mg_audio_play_sound_internal(resource);
    ma_sound_set_volume(sound, volume);
    ma_sound_set_pitch(sound, pitch);
}