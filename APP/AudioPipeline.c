#include "AudioPipeline.h"
#include "model/effect_chain.h"
#include "i2s.h"

#include "effects/amp_sim/amp_sim.h"
#include "effects/cab_sim/cab_sim.h"
#include "effects/reverb/reverb.h"

static int16_t i2s_rx_buffer[AUDIO_BUFFER_SIZE * 2 * 2];
static int16_t i2s_tx_buffer[AUDIO_BUFFER_SIZE * 2 * 2];
static float    dsp_buffer[AUDIO_BUFFER_SIZE];

static volatile uint8_t pending_half;
static volatile uint8_t pending_flag;

static void Process_Half(uint8_t half)
{
    const int16_t *src = &i2s_rx_buffer[half * AUDIO_BUFFER_SIZE * 2];

    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
        dsp_buffer[i] = (float)src[i * 2] / 32768.0f;

    // EffectChain_Process(dsp_buffer, AUDIO_BUFFER_SIZE);

    int16_t *dst = &i2s_tx_buffer[half * AUDIO_BUFFER_SIZE * 2];
    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        int16_t s = (int16_t)(dsp_buffer[i] * 32767.0f);
        dst[i * 2]     = s;
        dst[i * 2 + 1] = s;
    }
}

void AudioPipeline_Init(void)
{
    EffectChain_Init();
    EffectChain_Add(&amp_sim_effect);
    EffectChain_Add(&reverb_effect);
    EffectChain_Add(&cab_sim_effect);

    HAL_I2SEx_TransmitReceive_DMA(&hi2s2,
        (uint16_t *)i2s_tx_buffer,
        (uint16_t *)i2s_rx_buffer,
        AUDIO_BUFFER_SIZE * 2 * 2);
}

void AudioPipeline_Tick(void)
{
    if (!pending_flag) return;
    pending_flag = 0;
    Process_Half(pending_half);
}

void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        pending_half = 0;
        pending_flag = 1;
    }
}

void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        pending_half = 1;
        pending_flag = 1;
    }
}
