#pragma once

#include "base.h"

#include <miniaudio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_sound_resource
{
    ma_sound internal_data;
}
mg_sound_resource_t;

typedef struct mg_sound
{
    ma_sound *internal_data;
}
mg_sound_t;

MG_API void mg_audio_initialize (void);
MG_API void mg_audio_shutdown   (void);

MG_API mg_sound_resource_t *mg_audio_create_sound_resource_from_file    (const char *file_name);
MG_API void                 mg_audio_destroy_sound_resource             (mg_sound_resource_t *sound);

MG_API void mg_audio_play_sound         (mg_sound_resource_t *resource, mg_sound_t *out_sound);

MG_API void mg_audio_set_sound_volume   (mg_sound_t sound, float volume);
MG_API void mg_audio_set_sound_pitch    (mg_sound_t sound, float pitch);
MG_API void mg_audio_set_sound_position (mg_sound_t sound, float x, float y, float z);

#ifdef __cplusplus
}
#endif