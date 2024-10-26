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
mg_sound_resource;

typedef struct mg_sound mg_sound;

MG_API void mg_audio_initialize (void);
MG_API void mg_audio_shutdown   (void);

MG_API mg_sound_resource *mg_audio_create_sound_resource_from_file    (const char *file_name);
MG_API void                 mg_audio_destroy_sound_resource             (mg_sound_resource *sound);

MG_API mg_sound          *mg_audio_play_sound                         (mg_sound_resource *resource);

MG_API void                 mg_audio_set_sound_volume                   (mg_sound *sound, float volume);
MG_API void                 mg_audio_set_sound_pitch                    (mg_sound *sound, float pitch);
MG_API void                 mg_audio_set_sound_position                 (mg_sound *sound, float x, float y, float z);

#ifdef __cplusplus
}
#endif