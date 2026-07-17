#include "tone_test.h"
#include "i2s.h"
#include "main.h"
#include <math.h>

static int16_t tone_tx[AUDIO_BUFFER_SIZE * 2 * 2];
static int16_t tone_rx[AUDIO_BUFFER_SIZE * 2 * 2];

void ToneTest_Start(void)
{
    const float freq = 440.0f;
    const float sr   = 44100.0f;
    const float step = 2.0f * 3.14159265359f * freq / sr;
    float       phase = 0.0f;

    for (int i = 0; i < AUDIO_BUFFER_SIZE * 2; i++)
    {
        int16_t s = (int16_t)(sinf(phase) * 16383.0f);
        tone_tx[i * 2]     = s;
        tone_tx[i * 2 + 1] = s;
        phase += step;
    }

    HAL_I2SEx_TransmitReceive_DMA(&hi2s2,
        (uint16_t *)tone_tx,
        (uint16_t *)tone_rx,
        AUDIO_BUFFER_SIZE * 2 * 2);
}
