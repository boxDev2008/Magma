#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef MGAUDIO_API
#if defined(_WIN32) && defined(MG_DLL) && (defined(MG_IMPL) || defined(MGAUDIO_IMPL))
#define MGAUDIO_API __declspec(dllexport)
#elif defined(_WIN32) && defined(MG_DLL)
#define MGAUDIO_API __declspec(dllimport)
#else
#define MGAUDIO_API extern
#endif
#endif

#ifndef MGAUDIO_DEFAULT_SAMPLE_RATE
#define MGAUDIO_DEFAULT_SAMPLE_RATE 48000
#endif

#ifndef MGAUDIO_DEFAULT_CHANNELS
#define MGAUDIO_DEFAULT_CHANNELS 2
#endif

#ifndef MGAUDIO_DEFAULT_BUFFER_FRAMES
#define MGAUDIO_DEFAULT_BUFFER_FRAMES 512
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t mgaudio_result;
enum
{
    MGAUDIO_RESULT_SUCCESS,
    MGAUDIO_RESULT_FAILURE
};

typedef uint8_t mgaudio_wav_format;
enum
{
    MGAUDIO_WAV_FORMAT_PCM16,
    MGAUDIO_WAV_FORMAT_PCM24,
    MGAUDIO_WAV_FORMAT_FLOAT32
};

typedef void (*mgaudio_data_fn)(float *output, uint32_t frames, void *user_data);

typedef struct
{
    mgaudio_data_fn data_callback;
    void *user_data;
    uint32_t sample_rate;
    uint32_t channels;
    uint32_t buffer_frames;
}
mgaudio_init_info;

typedef struct
{
    float *samples;
    uint32_t sample_rate;
    uint32_t channels;
    uint64_t frames;
}
mgaudio_wav;

MGAUDIO_API mgaudio_result mgaudio_init(const mgaudio_init_info *init_info);
MGAUDIO_API void mgaudio_shutdown(void);

MGAUDIO_API void mgaudio_start(void);
MGAUDIO_API void mgaudio_stop(void);

MGAUDIO_API uint32_t mgaudio_sample_rate(void);
MGAUDIO_API uint32_t mgaudio_channels(void);

MGAUDIO_API mgaudio_result mgaudio_wav_decode(const void *data, size_t size, mgaudio_wav *wav);
MGAUDIO_API mgaudio_result mgaudio_wav_load(const char *path, mgaudio_wav *wav);
MGAUDIO_API void mgaudio_wav_free(mgaudio_wav *wav);

MGAUDIO_API void *mgaudio_wav_encode(const float *samples, uint64_t frames, uint32_t channels, uint32_t sample_rate, mgaudio_wav_format format, size_t *out_size);
MGAUDIO_API mgaudio_result mgaudio_wav_save(const char *path, const float *samples, uint64_t frames, uint32_t channels, uint32_t sample_rate, mgaudio_wav_format format);

#ifdef __cplusplus
} // extern "C"

inline mgaudio_result mgaudio_init(const mgaudio_init_info &init_info) { return mgaudio_init(&init_info); }

#endif

#if defined(MG_IMPL) || defined(MGAUDIO_IMPL)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MGAUDIO_ERROR(msg) \
    fprintf(stderr, "[MGAUDIO]: Error: %s\nLine: %d\n", msg, __LINE__);

#if defined(__EMSCRIPTEN__)

#include <emscripten.h>

#elif defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef COBJMACROS
#define COBJMACROS
#endif
#include <windows.h>
#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#pragma comment(lib, "ole32")

DEFINE_GUID(MGAUDIO_CLSID_MMDeviceEnumerator, 0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E);
DEFINE_GUID(MGAUDIO_IID_IMMDeviceEnumerator, 0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6);
DEFINE_GUID(MGAUDIO_IID_IAudioClient, 0x1CB9AD4C, 0xDBFA, 0x4C32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2);
DEFINE_GUID(MGAUDIO_IID_IAudioRenderClient, 0xF294ACFC, 0x3146, 0x4483, 0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2);

#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
#endif
#ifndef AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
#define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY 0x08000000
#endif

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

typedef struct
{
    IMMDeviceEnumerator *enumerator;
    IMMDevice *device;
    IAudioClient *client;
    IAudioRenderClient *render_client;

    HANDLE thread;
    HANDLE event;

    UINT32 buffer_frames;
    bool com_initialized;
}
mgaudio_win32_state;

#elif defined(__linux__)

#include <dlfcn.h>
#include <pthread.h>

typedef struct _snd_pcm snd_pcm_t;
typedef unsigned long snd_pcm_uframes_t;
typedef long snd_pcm_sframes_t;

#define MGAUDIO_SND_PCM_STREAM_PLAYBACK 0
#define MGAUDIO_SND_PCM_FORMAT_FLOAT_LE 14

typedef struct
{
    void *lib;
    snd_pcm_t *pcm;
    pthread_t thread;

    int (*snd_pcm_open)(snd_pcm_t **, const char *, int, int);
    int (*snd_pcm_close)(snd_pcm_t *);
    int (*snd_pcm_set_params)(snd_pcm_t *, int format, int access, unsigned channels, unsigned rate, int soft_resample, unsigned latency_us);
    snd_pcm_sframes_t (*snd_pcm_writei)(snd_pcm_t *, const void *, snd_pcm_uframes_t);
    int (*snd_pcm_recover)(snd_pcm_t *, int err, int silent);
    int (*snd_pcm_drop)(snd_pcm_t *);
    int (*snd_pcm_prepare)(snd_pcm_t *);
}
mgaudio_alsa_state;

#endif

typedef struct
{
#if defined(__EMSCRIPTEN__)
    int unused;
#elif defined(_WIN32)
    mgaudio_win32_state win32;
#elif defined(__linux__)
    mgaudio_alsa_state alsa;
#endif

    mgaudio_data_fn data;
    void *user_data;

    float *mix_buffer;

    uint32_t sample_rate;
    uint32_t channels;
    uint32_t buffer_frames;

    volatile bool running;
    volatile bool playing;
    bool initialized;
}
mgaudio_core_state;
static mgaudio_core_state mgaudio_state;

static inline void mgaudio_render(float *output, uint32_t frames)
{
    memset(output, 0, (size_t)frames * mgaudio_state.channels * sizeof(float));
    if (mgaudio_state.playing && mgaudio_state.data)
        mgaudio_state.data(output, frames, mgaudio_state.user_data);
}

uint32_t mgaudio_sample_rate(void)
{
    return mgaudio_state.sample_rate;
}

uint32_t mgaudio_channels(void)
{
    return mgaudio_state.channels;
}

void mgaudio_start(void)
{
    mgaudio_state.playing = true;
}

void mgaudio_stop(void)
{
    mgaudio_state.playing = false;
}

static inline void mgaudio_fill_defaults(mgaudio_init_info *info)
{
    if (!info->sample_rate)
        info->sample_rate = MGAUDIO_DEFAULT_SAMPLE_RATE;
    if (!info->channels)
        info->channels = MGAUDIO_DEFAULT_CHANNELS;
    if (!info->buffer_frames)
        info->buffer_frames = MGAUDIO_DEFAULT_BUFFER_FRAMES;
}

#if defined(__EMSCRIPTEN__)

EMSCRIPTEN_KEEPALIVE void mgaudio_emscripten_render(float *output, uint32_t frames)
{
    mgaudio_render(output, frames);
}

mgaudio_result mgaudio_init(const mgaudio_init_info *init_info)
{
    mgaudio_init_info info = *init_info;
    mgaudio_fill_defaults(&info);

    mgaudio_state.data = info.data_callback;
    mgaudio_state.user_data = info.user_data;
    mgaudio_state.channels = info.channels;
    mgaudio_state.buffer_frames = info.buffer_frames;
    mgaudio_state.mix_buffer = (float*)calloc((size_t)info.buffer_frames * info.channels, sizeof(float));

    const int32_t sample_rate = EM_ASM_INT({
        var AC = window.AudioContext || window.webkitAudioContext;
        if (!AC)
            return 0;

        var ctx = new AC({ sampleRate: $0 });
        var node = ctx.createScriptProcessor($1, 0, $2);
        var frames = $1;
        var channels = $2;
        var ptr = $3;

        node.onaudioprocess = function(e)
        {
            _mgaudio_emscripten_render(ptr, frames);
            for (var c = 0; c < channels; c++)
            {
                var out = e.outputBuffer.getChannelData(c);
                for (var i = 0; i < frames; i++)
                    out[i] = HEAPF32[(ptr >> 2) + i * channels + c];
            }
        };

        node.connect(ctx.destination);
        Module.mgaudio = { ctx: ctx, node: node };

        var resume = function()
        {
            if (ctx.state !== "running")
                ctx.resume();
        };
        window.addEventListener("keydown", resume);
        window.addEventListener("mousedown", resume);
        window.addEventListener("touchstart", resume);

        return ctx.sampleRate;
    }, info.sample_rate, info.buffer_frames, info.channels, mgaudio_state.mix_buffer);

    if (!sample_rate)
    {
        MGAUDIO_ERROR("Web Audio is not available");
        free(mgaudio_state.mix_buffer);
        mgaudio_state.mix_buffer = NULL;
        return MGAUDIO_RESULT_FAILURE;
    }

    mgaudio_state.sample_rate = (uint32_t)sample_rate;
    mgaudio_state.playing = true;
    mgaudio_state.initialized = true;
    return MGAUDIO_RESULT_SUCCESS;
}

void mgaudio_shutdown(void)
{
    if (!mgaudio_state.initialized)
        return;

    EM_ASM({
        if (Module.mgaudio)
        {
            Module.mgaudio.node.disconnect();
            Module.mgaudio.ctx.close();
            Module.mgaudio = null;
        }
    });

    free(mgaudio_state.mix_buffer);
    memset(&mgaudio_state, 0, sizeof(mgaudio_state));
}

#elif defined(_WIN32)

static DWORD WINAPI mgaudio_win32_thread(LPVOID param)
{
    mgaudio_win32_state *win32_state = &mgaudio_state.win32;
    const uint32_t channels = mgaudio_state.channels;

    while (mgaudio_state.running)
    {
        if (WaitForSingleObject(win32_state->event, 200) != WAIT_OBJECT_0)
            continue;

        UINT32 padding = 0;
        if (FAILED(IAudioClient_GetCurrentPadding(win32_state->client, &padding)))
            continue;

        const UINT32 frames = win32_state->buffer_frames - padding;
        if (frames == 0)
            continue;

        BYTE *data = NULL;
        if (FAILED(IAudioRenderClient_GetBuffer(win32_state->render_client, frames, &data)))
            continue;

        mgaudio_render((float*)data, frames);
        IAudioRenderClient_ReleaseBuffer(win32_state->render_client, frames, 0);
    }

    (void)channels;
    return 0;
}

mgaudio_result mgaudio_init(const mgaudio_init_info *init_info)
{
    mgaudio_init_info info = *init_info;
    mgaudio_fill_defaults(&info);

    mgaudio_win32_state *win32_state = &mgaudio_state.win32;

    mgaudio_state.data = info.data_callback;
    mgaudio_state.user_data = info.user_data;
    mgaudio_state.sample_rate = info.sample_rate;
    mgaudio_state.channels = info.channels;
    mgaudio_state.buffer_frames = info.buffer_frames;

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    win32_state->com_initialized = SUCCEEDED(hr);

    hr = CoCreateInstance(&MGAUDIO_CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &MGAUDIO_IID_IMMDeviceEnumerator, (void**)&win32_state->enumerator);
    if (FAILED(hr))
    {
        MGAUDIO_ERROR("Failed to create device enumerator");
        return MGAUDIO_RESULT_FAILURE;
    }

    hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(win32_state->enumerator, eRender, eConsole, &win32_state->device);
    if (FAILED(hr))
    {
        MGAUDIO_ERROR("Failed to get default audio endpoint");
        mgaudio_shutdown();
        return MGAUDIO_RESULT_FAILURE;
    }

    hr = IMMDevice_Activate(win32_state->device, &MGAUDIO_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&win32_state->client);
    if (FAILED(hr))
    {
        MGAUDIO_ERROR("Failed to activate audio client");
        mgaudio_shutdown();
        return MGAUDIO_RESULT_FAILURE;
    }

    WAVEFORMATEX format = {0};
    format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    format.nChannels = (WORD)info.channels;
    format.nSamplesPerSec = info.sample_rate;
    format.wBitsPerSample = 32;
    format.nBlockAlign = (WORD)(format.nChannels * format.wBitsPerSample / 8);
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

    const REFERENCE_TIME duration = (REFERENCE_TIME)info.buffer_frames * 3 * 10000000 / info.sample_rate;

    hr = IAudioClient_Initialize(
        win32_state->client,
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
        duration, 0, &format, NULL);
    if (FAILED(hr))
    {
        MGAUDIO_ERROR("Failed to initialize audio client");
        mgaudio_shutdown();
        return MGAUDIO_RESULT_FAILURE;
    }

    win32_state->event = CreateEventA(NULL, FALSE, FALSE, NULL);
    IAudioClient_SetEventHandle(win32_state->client, win32_state->event);
    IAudioClient_GetBufferSize(win32_state->client, &win32_state->buffer_frames);

    hr = IAudioClient_GetService(win32_state->client, &MGAUDIO_IID_IAudioRenderClient, (void**)&win32_state->render_client);
    if (FAILED(hr))
    {
        MGAUDIO_ERROR("Failed to get audio render client");
        mgaudio_shutdown();
        return MGAUDIO_RESULT_FAILURE;
    }

    BYTE *data = NULL;
    if (SUCCEEDED(IAudioRenderClient_GetBuffer(win32_state->render_client, win32_state->buffer_frames, &data)))
        IAudioRenderClient_ReleaseBuffer(win32_state->render_client, win32_state->buffer_frames, AUDCLNT_BUFFERFLAGS_SILENT);

    mgaudio_state.playing = true;
    mgaudio_state.running = true;
    mgaudio_state.initialized = true;

    win32_state->thread = CreateThread(NULL, 0, mgaudio_win32_thread, NULL, 0, NULL);
    IAudioClient_Start(win32_state->client);

    return MGAUDIO_RESULT_SUCCESS;
}

void mgaudio_shutdown(void)
{
    mgaudio_win32_state *win32_state = &mgaudio_state.win32;

    mgaudio_state.running = false;

    if (win32_state->thread)
    {
        WaitForSingleObject(win32_state->thread, INFINITE);
        CloseHandle(win32_state->thread);
    }

    if (win32_state->client)
        IAudioClient_Stop(win32_state->client);
    if (win32_state->render_client)
        IAudioRenderClient_Release(win32_state->render_client);
    if (win32_state->client)
        IAudioClient_Release(win32_state->client);
    if (win32_state->device)
        IMMDevice_Release(win32_state->device);
    if (win32_state->enumerator)
        IMMDeviceEnumerator_Release(win32_state->enumerator);
    if (win32_state->event)
        CloseHandle(win32_state->event);
    if (win32_state->com_initialized)
        CoUninitialize();

    memset(&mgaudio_state, 0, sizeof(mgaudio_state));
}

#elif defined(__linux__)

static void *mgaudio_alsa_thread(void *param)
{
    mgaudio_alsa_state *alsa_state = &mgaudio_state.alsa;
    const uint32_t frames = mgaudio_state.buffer_frames;

    while (mgaudio_state.running)
    {
        mgaudio_render(mgaudio_state.mix_buffer, frames);

        snd_pcm_sframes_t written = alsa_state->snd_pcm_writei(alsa_state->pcm, mgaudio_state.mix_buffer, frames);
        if (written < 0)
            alsa_state->snd_pcm_recover(alsa_state->pcm, (int)written, 1);
    }

    return NULL;
}

mgaudio_result mgaudio_init(const mgaudio_init_info *init_info)
{
    mgaudio_init_info info = *init_info;
    mgaudio_fill_defaults(&info);

    mgaudio_alsa_state *alsa_state = &mgaudio_state.alsa;

    mgaudio_state.data = info.data_callback;
    mgaudio_state.user_data = info.user_data;
    mgaudio_state.sample_rate = info.sample_rate;
    mgaudio_state.channels = info.channels;
    mgaudio_state.buffer_frames = info.buffer_frames;

    alsa_state->lib = dlopen("libasound.so.2", RTLD_LAZY | RTLD_LOCAL);
    if (!alsa_state->lib)
        alsa_state->lib = dlopen("libasound.so", RTLD_LAZY | RTLD_LOCAL);
    if (!alsa_state->lib)
    {
        MGAUDIO_ERROR("Failed to load libasound");
        return MGAUDIO_RESULT_FAILURE;
    }

#define MGAUDIO_ALSA_LOAD(name) alsa_state->name = dlsym(alsa_state->lib, #name)
    MGAUDIO_ALSA_LOAD(snd_pcm_open);
    MGAUDIO_ALSA_LOAD(snd_pcm_close);
    MGAUDIO_ALSA_LOAD(snd_pcm_set_params);
    MGAUDIO_ALSA_LOAD(snd_pcm_writei);
    MGAUDIO_ALSA_LOAD(snd_pcm_recover);
    MGAUDIO_ALSA_LOAD(snd_pcm_drop);
    MGAUDIO_ALSA_LOAD(snd_pcm_prepare);
#undef MGAUDIO_ALSA_LOAD

    if (!alsa_state->snd_pcm_open || !alsa_state->snd_pcm_set_params || !alsa_state->snd_pcm_writei)
    {
        MGAUDIO_ERROR("Failed to load ALSA functions");
        dlclose(alsa_state->lib);
        memset(&mgaudio_state, 0, sizeof(mgaudio_state));
        return MGAUDIO_RESULT_FAILURE;
    }

    if (alsa_state->snd_pcm_open(&alsa_state->pcm, "default", MGAUDIO_SND_PCM_STREAM_PLAYBACK, 0) < 0)
    {
        MGAUDIO_ERROR("Failed to open ALSA device");
        dlclose(alsa_state->lib);
        memset(&mgaudio_state, 0, sizeof(mgaudio_state));
        return MGAUDIO_RESULT_FAILURE;
    }

    const unsigned latency = (unsigned)((uint64_t)info.buffer_frames * 3 * 1000000 / info.sample_rate);
    if (alsa_state->snd_pcm_set_params(alsa_state->pcm, MGAUDIO_SND_PCM_FORMAT_FLOAT_LE, 3, info.channels, info.sample_rate, 1, latency) < 0)
    {
        MGAUDIO_ERROR("Failed to set ALSA parameters");
        alsa_state->snd_pcm_close(alsa_state->pcm);
        dlclose(alsa_state->lib);
        memset(&mgaudio_state, 0, sizeof(mgaudio_state));
        return MGAUDIO_RESULT_FAILURE;
    }

    mgaudio_state.mix_buffer = (float*)calloc((size_t)info.buffer_frames * info.channels, sizeof(float));

    mgaudio_state.playing = true;
    mgaudio_state.running = true;
    mgaudio_state.initialized = true;

    pthread_create(&alsa_state->thread, NULL, mgaudio_alsa_thread, NULL);
    return MGAUDIO_RESULT_SUCCESS;
}

void mgaudio_shutdown(void)
{
    mgaudio_alsa_state *alsa_state = &mgaudio_state.alsa;

    if (!mgaudio_state.initialized)
        return;

    mgaudio_state.running = false;
    pthread_join(alsa_state->thread, NULL);

    if (alsa_state->snd_pcm_drop)
        alsa_state->snd_pcm_drop(alsa_state->pcm);
    alsa_state->snd_pcm_close(alsa_state->pcm);
    dlclose(alsa_state->lib);

    free(mgaudio_state.mix_buffer);
    memset(&mgaudio_state, 0, sizeof(mgaudio_state));
}

#endif

static inline uint16_t mgaudio_read_u16(const uint8_t *p)
{
    return (uint16_t)(p[0] | (p[1] << 8));
}

static inline uint32_t mgaudio_read_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline void mgaudio_write_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)(v >> 8);
}

static inline void mgaudio_write_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)(v >> 24);
}

static inline float mgaudio_clampf(float v)
{
    return v < -1.0f ? -1.0f : (v > 1.0f ? 1.0f : v);
}

mgaudio_result mgaudio_wav_decode(const void *data, size_t size, mgaudio_wav *wav)
{
    const uint8_t *bytes = (const uint8_t*)data;

    if (!data || !wav || size < 12)
        return MGAUDIO_RESULT_FAILURE;
    if (memcmp(bytes, "RIFF", 4) != 0 || memcmp(bytes + 8, "WAVE", 4) != 0)
        return MGAUDIO_RESULT_FAILURE;

    uint16_t format_tag = 0;
    uint16_t channels = 0;
    uint16_t bits = 0;
    uint32_t sample_rate = 0;

    const uint8_t *pcm = NULL;
    uint32_t pcm_size = 0;
    bool has_format = false;

    size_t offset = 12;
    while (offset + 8 <= size)
    {
        const uint8_t *chunk = bytes + offset;
        uint32_t chunk_size = mgaudio_read_u32(chunk + 4);
        const size_t body = offset + 8;

        if (body + chunk_size > size)
            chunk_size = (uint32_t)(size - body);

        if (memcmp(chunk, "fmt ", 4) == 0 && chunk_size >= 16)
        {
            format_tag = mgaudio_read_u16(chunk + 8);
            channels = mgaudio_read_u16(chunk + 10);
            sample_rate = mgaudio_read_u32(chunk + 12);
            bits = mgaudio_read_u16(chunk + 22);

            if (format_tag == 0xFFFE && chunk_size >= 40)
                format_tag = mgaudio_read_u16(chunk + 8 + 24);

            has_format = true;
        }
        else if (memcmp(chunk, "data", 4) == 0)
        {
            pcm = bytes + body;
            pcm_size = chunk_size;
        }

        offset = body + chunk_size + (chunk_size & 1);
    }

    if (!has_format || !pcm || channels == 0)
        return MGAUDIO_RESULT_FAILURE;

    const bool is_pcm = format_tag == 1 && (bits == 16 || bits == 24);
    const bool is_float = format_tag == 3 && bits == 32;
    if (!is_pcm && !is_float)
        return MGAUDIO_RESULT_FAILURE;

    const uint32_t bytes_per_sample = bits / 8;
    const uint64_t frames = pcm_size / (bytes_per_sample * channels);
    const uint64_t sample_count = frames * channels;

    float *samples = (float*)malloc((size_t)(sample_count ? sample_count : 1) * sizeof(float));
    if (!samples)
        return MGAUDIO_RESULT_FAILURE;

    for (uint64_t i = 0; i < sample_count; i++)
    {
        const uint8_t *p = pcm + i * bytes_per_sample;

        if (bits == 16)
            samples[i] = (float)(int16_t)mgaudio_read_u16(p) / 32768.0f;
        else if (bits == 24)
        {
            int32_t v = (int32_t)((uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16));
            if (v & 0x800000)
                v |= (int32_t)0xFF000000;
            samples[i] = (float)v / 8388608.0f;
        }
        else
        {
            uint32_t v = mgaudio_read_u32(p);
            float f;
            memcpy(&f, &v, sizeof(f));
            samples[i] = f;
        }
    }

    wav->samples = samples;
    wav->sample_rate = sample_rate;
    wav->channels = channels;
    wav->frames = frames;
    return MGAUDIO_RESULT_SUCCESS;
}

static inline FILE *mgaudio_fopen(const char *path, const char *mode)
{
#if defined(_WIN32)
    FILE *file = NULL;
    if (fopen_s(&file, path, mode) != 0)
        return NULL;
    return file;
#else
    return fopen(path, mode);
#endif
}

mgaudio_result mgaudio_wav_load(const char *path, mgaudio_wav *wav)
{
    FILE *file = mgaudio_fopen(path, "rb");
    if (!file)
        return MGAUDIO_RESULT_FAILURE;

    fseek(file, 0, SEEK_END);
    const long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size <= 0)
    {
        fclose(file);
        return MGAUDIO_RESULT_FAILURE;
    }

    uint8_t *buffer = (uint8_t*)malloc((size_t)size);
    if (!buffer)
    {
        fclose(file);
        return MGAUDIO_RESULT_FAILURE;
    }

    const size_t read = fread(buffer, 1, (size_t)size, file);
    fclose(file);

    mgaudio_result result = MGAUDIO_RESULT_FAILURE;
    if (read == (size_t)size)
        result = mgaudio_wav_decode(buffer, (size_t)size, wav);

    free(buffer);
    return result;
}

void mgaudio_wav_free(mgaudio_wav *wav)
{
    if (!wav)
        return;
    free(wav->samples);
    memset(wav, 0, sizeof(*wav));
}

void *mgaudio_wav_encode(const float *samples, uint64_t frames, uint32_t channels, uint32_t sample_rate, mgaudio_wav_format format, size_t *out_size)
{
    if (!samples || channels == 0 || sample_rate == 0)
        return NULL;

    uint16_t format_tag = 1;
    uint16_t bits = 16;
    switch (format)
    {
        case MGAUDIO_WAV_FORMAT_PCM16:   format_tag = 1; bits = 16; break;
        case MGAUDIO_WAV_FORMAT_PCM24:   format_tag = 1; bits = 24; break;
        case MGAUDIO_WAV_FORMAT_FLOAT32: format_tag = 3; bits = 32; break;
    }

    const uint32_t bytes_per_sample = bits / 8;
    const uint64_t sample_count = frames * channels;
    const uint64_t data_size = sample_count * bytes_per_sample;

    if (data_size > 0xFFFFFFFFull - 36)
        return NULL;

    const size_t total = 44 + (size_t)data_size;
    uint8_t *out = (uint8_t*)malloc(total);
    if (!out)
        return NULL;

    memcpy(out, "RIFF", 4);
    mgaudio_write_u32(out + 4, (uint32_t)(36 + data_size));
    memcpy(out + 8, "WAVE", 4);

    memcpy(out + 12, "fmt ", 4);
    mgaudio_write_u32(out + 16, 16);
    mgaudio_write_u16(out + 20, format_tag);
    mgaudio_write_u16(out + 22, (uint16_t)channels);
    mgaudio_write_u32(out + 24, sample_rate);
    mgaudio_write_u32(out + 28, sample_rate * channels * bytes_per_sample);
    mgaudio_write_u16(out + 32, (uint16_t)(channels * bytes_per_sample));
    mgaudio_write_u16(out + 34, bits);

    memcpy(out + 36, "data", 4);
    mgaudio_write_u32(out + 40, (uint32_t)data_size);

    uint8_t *p = out + 44;
    for (uint64_t i = 0; i < sample_count; i++)
    {
        if (bits == 16)
        {
            const int32_t v = (int32_t)(mgaudio_clampf(samples[i]) * 32767.0f + (samples[i] < 0.0f ? -0.5f : 0.5f));
            mgaudio_write_u16(p, (uint16_t)(int16_t)v);
        }
        else if (bits == 24)
        {
            const int32_t v = (int32_t)(mgaudio_clampf(samples[i]) * 8388607.0f + (samples[i] < 0.0f ? -0.5f : 0.5f));
            p[0] = (uint8_t)(v & 0xFF);
            p[1] = (uint8_t)((v >> 8) & 0xFF);
            p[2] = (uint8_t)((v >> 16) & 0xFF);
        }
        else
        {
            uint32_t v;
            memcpy(&v, &samples[i], sizeof(v));
            mgaudio_write_u32(p, v);
        }
        p += bytes_per_sample;
    }

    if (out_size)
        *out_size = total;
    return out;
}

mgaudio_result mgaudio_wav_save(const char *path, const float *samples, uint64_t frames, uint32_t channels, uint32_t sample_rate, mgaudio_wav_format format)
{
    size_t size = 0;
    void *data = mgaudio_wav_encode(samples, frames, channels, sample_rate, format, &size);
    if (!data)
        return MGAUDIO_RESULT_FAILURE;

    FILE *file = mgaudio_fopen(path, "wb");
    if (!file)
    {
        free(data);
        return MGAUDIO_RESULT_FAILURE;
    }

    const size_t written = fwrite(data, 1, size, file);
    fclose(file);
    free(data);

    return written == size ? MGAUDIO_RESULT_SUCCESS : MGAUDIO_RESULT_FAILURE;
}

#endif // MGAUDIO_IMPL