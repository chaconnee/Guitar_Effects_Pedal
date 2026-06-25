#include "AudioPipeline.h"
#include "model/effect_chain.h"
#include "i2s.h"
#include <math.h>

#include "effects/amp_sim/amp_sim.h"
#include "effects/cab_sim/cab_sim.h"

/* I2S RX buffer: stereo, 16-bit signed, same size as TX buffer */
static int16_t i2s_rx_buffer[AUDIO_BUFFER_SIZE * 2 * 2];
/* I2S TX buffer: stereo, 16-bit signed */
static int16_t i2s_tx_buffer[AUDIO_BUFFER_SIZE * 2 * 2];
static float    dsp_buffer[AUDIO_BUFFER_SIZE];

static volatile uint8_t rx_half;
static volatile uint8_t rx_pending;

static float master_volume = 0.3f;

static void Process_Half(uint8_t half)
{
    /* Extract mono from stereo I2S RX (left channel only) */
    const int16_t *src = &i2s_rx_buffer[half * AUDIO_BUFFER_SIZE * 2];
    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        dsp_buffer[i] = (float)src[i * 2] / 32768.0f;
    }

    EffectChain_Process(dsp_buffer, AUDIO_BUFFER_SIZE);

    /* Soft clipping */
    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        dsp_buffer[i] = tanhf(dsp_buffer[i]);
    }

    /* Master volume */
    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        dsp_buffer[i] *= master_volume;
    }

    /* Write stereo output to I2S TX buffer */
    int16_t *dst = &i2s_tx_buffer[half * AUDIO_BUFFER_SIZE * 2];
    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        int16_t s = (int16_t)(dsp_buffer[i] * 32767.0f);
        dst[i * 2]     = s;  /* Left */
        dst[i * 2 + 1] = s;  /* Right */
    }
}

void AudioPipeline_Init(void)
{
    EffectChain_Init();
    EffectChain_Add(&amp_sim_effect);
    EffectChain_Add(&cab_sim_effect);

    /* Start I2S RX DMA (receives ADC data from WM8978) */
    HAL_I2S_Receive_DMA(&hi2s2, (uint16_t *)i2s_rx_buffer, AUDIO_BUFFER_SIZE * 2 * 2);

    /* Start I2S TX DMA (sends DAC data to WM8978) */
    HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t *)i2s_tx_buffer, AUDIO_BUFFER_SIZE * 2 * 2);
}

void AudioPipeline_Tick(void)
{
    if (!rx_pending) return;
    rx_pending = 0;
    Process_Half(rx_half);
}

/* I2S RX half-transfer complete callback */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        rx_half = 0;
        rx_pending = 1;
    }
}

/* I2S RX full-transfer complete callback */
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        rx_half = 1;
        rx_pending = 1;
    }
}
