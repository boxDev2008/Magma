#pragma once

#include <magma/audio/audio.h>
#include <afterglow/renderer.h>

#ifndef ASSET_MANAGER_ATLAS_WIDTH
    #define ASSET_MANAGER_ATLAS_WIDTH 1024
#endif
#ifndef ASSET_MANAGER_ATLAS_HEIGHT
    #define ASSET_MANAGER_ATLAS_HEIGHT 1024
#endif

void assets_initialize(void);
void assets_shutdown(void);

ag_sprite *assets_get_sprite(const char *name);
ag_font *assets_get_font(const char *name);
mg_sound_resource *assets_get_sound(const char *name);