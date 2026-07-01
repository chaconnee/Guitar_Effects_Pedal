#include "AudioPipeline.h"
#include "model/effect_chain.h"
#include "i2s.h"
#include <math.h>

#include "effects/amp_sim/amp_sim.h"
#include "effects/cab_sim/cab_sim.h"
#include "effects/reverb/reverb.h"
#include "effects/wav_data.h"

#define SINE_TEST  1

static int16_t i2s_rx_buffer[AUDIO_BUFFER_SIZE * 2 * 2];
static int16_t i2s_tx_buffer[AUDIO_BUFFER_SIZE * 2 * 2];
static float    dsp_buffer[AUDIO_BUFFER_SIZE];

static volatile uint8_t tx_half;
static volatile uint8_t tx_pending;

static float master_volume = 0.3f;

static void Process_Half(uint8_t half)
{
#if SINE_TEST
    static float phase = 0.0f;
    int16_t *dst = &i2s_tx_buffer[half * AUDIO_BUFFER_SIZE * 2];

    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        float s = sinf(phase) * 0.3f;
        phase += 2.0f * 3.14159265f * 1000.0f / 44100.0f;
        if (phase > 2.0f * 3.14159265f)
            phase -= 2.0f * 3.14159265f;

        int16_t val = (int16_t)(s * 32767.0f);
        dst[i * 2]     = val;
        dst[i * 2 + 1] = val;
    }
#else
    /* WAV 循环播放 + 效果处理 */
    static uint32_t wav_pos = 0;
    const int16_t *src = &i2s_rx_buffer[half * AUDIO_BUFFER_SIZE * 2];

    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        dsp_buffer[i] = (float)src[i * 2] / 32768.0f;
    }

    EffectChain_Process(dsp_buffer, AUDIO_BUFFER_SIZE);

    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        dsp_buffer[i] = tanhf(dsp_buffer[i]);
    }

    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        dsp_buffer[i] *= master_volume;
    }

    /* 混入 WAV 循环播放音轨 */
    int16_t *dst = &i2s_tx_buffer[half * AUDIO_BUFFER_SIZE * 2];
    for (uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        int32_t mixed = (int32_t)(dsp_buffer[i] * 32767.0f);
        mixed += (int32_t)wav_data[wav_pos] * 4;  /* 提升 12dB */
        wav_pos++;
        if (wav_pos >= WAV_DATA_LEN) wav_pos = 0;

        if (mixed >  32767) mixed =  32767;
        if (mixed < -32768) mixed = -32768;

        int16_t s = (int16_t)mixed;
        dst[i * 2]     = s;
        dst[i * 2 + 1] = s;
    }
#endif
}

void AudioPipeline_Init(void)
{
    EffectChain_Init();
    EffectChain_Add(&amp_sim_effect);
    EffectChain_Add(&reverb_effect);
    EffectChain_Add(&cab_sim_effect);

    /* 预填整个 TX 缓冲区 */
    Process_Half(0);
    Process_Half(1);

    /* 全双工 DMA: TX + RX 同步启动 (HAL 自动处理) */
    HAL_I2SEx_TransmitReceive_DMA(&hi2s2,
        (uint16_t *)i2s_tx_buffer,
        (uint16_t *)i2s_rx_buffer,
        AUDIO_BUFFER_SIZE * 2 * 2);
}

void AudioPipeline_Tick(void)
{
    if (!tx_pending) return;
    tx_pending = 0;
    Process_Half(tx_half);
}

/* TX half-transfer: 已发送 half0, 填充 half0 */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        tx_half = 0;
        tx_pending = 1;
    }
}

/* TX full-transfer: 已发送 half1, 填充 half1 */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        tx_half = 1;
        tx_pending = 1;
    }
}
