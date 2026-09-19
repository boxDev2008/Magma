#define MG_IMPL
#include "mgapp.h"
#include "mgaudio.h"

#include <math.h>

static float phase = 0.0f;

static void audio_callback(float *output, uint32_t frames, void *user_data)
{
    const uint32_t channels = mgaudio_channels();
    const float step = 440.0f / (float)mgaudio_sample_rate();

    for (uint32_t i = 0; i < frames; i++)
    {
        const float s = sinf(phase * 6.2831853f) * 0.2f;
        phase += step;
        if (phase >= 1.0f) phase -= 1.0f;

        for (uint32_t c = 0; c < channels; c++)
            output[i * channels + c] = s;
    }
}

void on_start(void)
{
    mgaudio_init(&(mgaudio_init_info){
         .data_callback = audio_callback
    });
}

void on_end(void)
{
    mgaudio_shutdown();
}

mgapp_init_info mgapp_main(void)
{
    return (mgapp_init_info) {
        .title = "Sound",
        .events = {
            .start = on_start,
            .end = on_end
        }
    };
}