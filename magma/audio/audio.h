#pragma once

#include "base.h"

#include <miniaudio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_sound
{
    ma_sound sound;
}
mg_sound_t;

MG_API void         mg_audio_initialize             (void);
MG_API void         mg_audio_shutdown               (void);

MG_API mg_sound_t  *mg_create_sound_from_file       (const char *file_name);
MG_API void         mg_destroy_sound                (mg_sound_t *sound);

MG_API mg_sound_t  *mg_audio_play_sound             (mg_sound_t *sound);
MG_API mg_sound_t  *mg_audio_play_sound_from_file   (const char *file_name);

#ifdef __cplusplus
}
#endif