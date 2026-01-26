#pragma once

#include <magma/audio/audio.h>
#include <magma-utils/hlrenderer/hlrenderer.h>

#ifndef ASSET_MANAGER_ATLAS_WIDTH
    #define ASSET_MANAGER_ATLAS_WIDTH 1024
#endif
#ifndef ASSET_MANAGER_ATLAS_HEIGHT
    #define ASSET_MANAGER_ATLAS_HEIGHT 1024
#endif

void mg_assets_initialize(void);
void mg_assets_shutdown(void);

mg_sprite *mg_assets_get_sprite(const char *name);
mg_sound_resource *mg_assets_get_sound(const char *name);
mg_font *mg_assets_get_default_font(void);