#include "assets.h"

#include <magma-utils/hlrenderer/stb_image.h>

#include <unordered_map>
#include <string>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#include "dirent.h"

static struct
{
    std::unordered_map<std::string, mg_sprite> sprites;
    std::unordered_map<std::string, mg_sound_resource*> sounds;
    std::unordered_map<std::string, mg_font> fonts;
    mg_texture atlas;
}
asset_manager;

static void strip_extension(char* filename, const char* extension)
{
    char* dot = strrchr(filename, '.');
    if (dot && strcmp(dot, extension) == 0)
        *dot = '\0';
}

#define SPRITE_PADDING 1

static void load_sprite_atlas(const char* folderPath)
{
    stbrp_context context;
    stbrp_node nodes[2048];
    stbrp_rect rects[2048];

    uint8_t* imageData[2048];
    char imageNames[2048][128];

    DIR* dir = opendir(folderPath);
    if (!dir)
    {
        fprintf(stderr, "Failed to open directory: %s\n", folderPath);
        return;
    }

    struct dirent* entry;
    int index = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".png"))
        {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", folderPath, entry->d_name);

            int w, h, c;
            uint8_t* data = stbi_load(fullPath, &w, &h, &c, 4);
            if (!data)
            {
                fprintf(stderr, "Failed to load image: %s\n", fullPath);
                continue;
            }

            imageData[index] = data;

            rects[index].id = index;
            rects[index].w = w + SPRITE_PADDING * 2;
            rects[index].h = h + SPRITE_PADDING * 2;

            strcpy(imageNames[index], entry->d_name);
            strip_extension(imageNames[index], ".png");

            ++index;
        }
    }

    closedir(dir);

    stbrp_init_target(&context, ASSET_MANAGER_ATLAS_WIDTH, ASSET_MANAGER_ATLAS_HEIGHT, nodes, 2048);
    if (!stbrp_pack_rects(&context, rects, index))
    {
        fprintf(stderr, "Failed to pack rectangles.\n");
        return;
    }

    size_t atlasSize = ASSET_MANAGER_ATLAS_WIDTH * ASSET_MANAGER_ATLAS_HEIGHT * 4;
    uint8_t *atlasData = (uint8_t*)calloc(1, atlasSize);

    for (int i = 0; i < index; ++i)
    {
        stbrp_rect* r = &rects[i];
        uint8_t* src = imageData[i];

        int w = r->w - SPRITE_PADDING * 2;
        int h = r->h - SPRITE_PADDING * 2;

        for (int y = 0; y < h; ++y)
        {
            uint8_t* srcRow = src + y * w * 4;
            uint8_t* dstRow = atlasData + ((r->y + y + SPRITE_PADDING) * ASSET_MANAGER_ATLAS_WIDTH + (r->x + SPRITE_PADDING)) * 4;
            memcpy(dstRow, srcRow, w * 4);
        }

        stbi_image_free(imageData[i]);
    }

    asset_manager.atlas = mg_hlgfx_add_texture(ASSET_MANAGER_ATLAS_WIDTH, ASSET_MANAGER_ATLAS_HEIGHT,
        MG_SAMPLER_FILTER_NEAREST, MG_PIXEL_FORMAT_RGBA8_UNORM, atlasData);

    free(atlasData);

    for (int i = 0; i < index; ++i)
    {
        stbrp_rect* r = &rects[i];
        int w = r->w - SPRITE_PADDING * 2;
        int h = r->h - SPRITE_PADDING * 2;

        mg_sprite sprite;
        sprite.texture_area.x = (float)(r->x + SPRITE_PADDING) / ASSET_MANAGER_ATLAS_WIDTH;
        sprite.texture_area.y = (float)(r->y + SPRITE_PADDING) / ASSET_MANAGER_ATLAS_HEIGHT;
        sprite.texture_area.z  = (float)w / ASSET_MANAGER_ATLAS_WIDTH;
        sprite.texture_area.w = (float)h / ASSET_MANAGER_ATLAS_HEIGHT;
        sprite.size.x = w;
        sprite.size.y = h;
        sprite.texture = &asset_manager.atlas;

        asset_manager.sprites[imageNames[i]] = sprite;
    }
}

static void load_sounds(const char* folderPath)
{
    DIR* dir = opendir(folderPath);
    if (!dir)
    {
        fprintf(stderr, "Failed to open directory: %s\n", folderPath);
        return;
    }

    struct dirent* entry;
    int index = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".wav"))
        {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", folderPath, entry->d_name);

            strip_extension(entry->d_name, ".wav");

            asset_manager.sounds[entry->d_name] = mg_audio_create_sound_resource_from_file(fullPath);
            ++index;
        }
    }

    closedir(dir);
}

static void load_fonts(const char* folderPath)
{
    DIR* dir = opendir(folderPath);
    if (!dir)
    {
        fprintf(stderr, "Failed to open directory: %s\n", folderPath);
        return;
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".ttf") || strstr(entry->d_name, ".otf"))
        {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", folderPath, entry->d_name);

            char fontName[128];
            strcpy(fontName, entry->d_name);
            strip_extension(fontName, ".ttf");
            strip_extension(fontName, ".otf");

            asset_manager.fonts[fontName] = mg_hlgfx_add_font_from_file(fullPath);
        }
    }

    closedir(dir);
}

void mg_assets_initialize(void)
{
    load_sprite_atlas("./assets/sprites");
    load_sounds("./assets/sounds");
    load_fonts("./assets/fonts");
}

void mg_assets_shutdown(void)
{
    for (auto& sound : asset_manager.sounds)
        mg_audio_destroy_sound_resource(sound.second);
}

mg_sprite *mg_assets_get_sprite(const char *name)
{
    mg_sprite* sprite = &asset_manager.sprites[name];
    return sprite;
}

mg_sound_resource *mg_assets_get_sound(const char *name)
{
    mg_sound_resource* sound = asset_manager.sounds[name];
    return sound;
}

mg_font *mg_assets_get_font(const char *name)
{
    mg_font *font = &asset_manager.fonts[name];
    return font;
}