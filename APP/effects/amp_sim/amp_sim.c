#include "amp_sim.h"
#include <math.h>

/*
 * AmpSim — 过载前级模拟
 *
 * 信号链: 增益 → 不对称 tanh → Tone Stack(3段 biquad) → 音量 → 限幅
 *
 * Tone Stack (参考 Will Pirkle OneMarkAmp):
 *   低搁架  fc=60Hz,   Q=0.707, range ±12dB
 *   中参量  fc=500Hz,  Q=1.0,   range ±6dB
 *   高搁架  fc=2400Hz, Q=0.707, range ±8dB
 *
 * 参数 (set_param):
 *   id=0: drive  0.0~1.0 (增益量, 越大过载越多)
 *   id=1: volume 0.0~1.0 (输出音量)
 *   id=2: bass   -12.0~12.0 (低搁架 dB)
 *   id=3: mid    -6.0~6.0   (中参量 dB)
 *   id=4: treble -8.0~8.0   (高搁架 dB)
 */

#define SAMPLE_RATE 32000.0f
#define TWO_PI      6.283185307179586f
#define DRIVE_MAX   25.0f

/* ── biquad Direct Form I 状态 ── */
typedef struct {
    float b0, b1, b2, a1, a2;
    float x1, x2, y1, y2;
} Biquad;

/* ── biquad 系数计算 ── */
static void biquad_low_shelf(Biquad *f, float fc, float Q, float dBgain)
{
    float w0 = TWO_PI * fc / SAMPLE_RATE;
    float A  = powf(10.0f, dBgain / 40.0f);
    float alpha = sinf(w0) / (2.0f * Q);
    float cosw  = cosf(w0);
    float sqrtA = sqrtf(A);

    float a0 = (A+1.0f) + (A-1.0f)*cosw + 2.0f*sqrtA*alpha;
    f->b0 = A * ((A+1.0f) - (A-1.0f)*cosw + 2.0f*sqrtA*alpha) / a0;
    f->b1 = 2.0f * A * ((A-1.0f) - (A+1.0f)*cosw) / a0;
    f->b2 = A * ((A+1.0f) - (A-1.0f)*cosw - 2.0f*sqrtA*alpha) / a0;
    f->a1 = -2.0f * ((A-1.0f) + (A+1.0f)*cosw) / a0;
    f->a2 = ((A+1.0f) + (A-1.0f)*cosw - 2.0f*sqrtA*alpha) / a0;
}

static void biquad_parametric(Biquad *f, float fc, float Q, float dBgain)
{
    float w0 = TWO_PI * fc / SAMPLE_RATE;
    float A  = powf(10.0f, dBgain / 40.0f);
    float alpha = sinf(w0) / (2.0f * Q);
    float cosw  = cosf(w0);

    float a0 = 1.0f + alpha / A;
    f->b0 = (1.0f + alpha * A) / a0;
    f->b1 = (-2.0f * cosw) / a0;
    f->b2 = (1.0f - alpha * A) / a0;
    f->a1 = (-2.0f * cosw) / a0;
    f->a2 = (1.0f - alpha / A) / a0;
}

static void biquad_high_shelf(Biquad *f, float fc, float Q, float dBgain)
{
    float w0 = TWO_PI * fc / SAMPLE_RATE;
    float A  = powf(10.0f, dBgain / 40.0f);
    float alpha = sinf(w0) / (2.0f * Q);
    float cosw  = cosf(w0);
    float sqrtA = sqrtf(A);

    float a0 = (A+1.0f) - (A-1.0f)*cosw + 2.0f*sqrtA*alpha;
    f->b0 = A * ((A+1.0f) + (A-1.0f)*cosw + 2.0f*sqrtA*alpha) / a0;
    f->b1 = -2.0f * A * ((A-1.0f) + (A+1.0f)*cosw) / a0;
    f->b2 = A * ((A+1.0f) + (A-1.0f)*cosw - 2.0f*sqrtA*alpha) / a0;
    f->a1 = 2.0f * ((A-1.0f) - (A+1.0f)*cosw) / a0;
    f->a2 = ((A+1.0f) - (A-1.0f)*cosw - 2.0f*sqrtA*alpha) / a0;
}

/* ── biquad 处理一个采样 ── */
static inline float biquad_tick(Biquad *f, float x)
{
    float y = f->b0*x + f->b1*f->x1 + f->b2*f->x2
            - f->a1*f->y1 - f->a2*f->y2;
    f->x2 = f->x1;  f->x1 = x;
    f->y2 = f->y1;  f->y1 = y;
    return y;
}

/* ── 滤波器实例 ── */
static Biquad tone_bass, tone_mid, tone_treble;

/* ── 用户参数 (Fender 玻璃声默认值) ── */
static float drive    = 0.0f;
static float volume   = 0.9f;
static float bass_db  = 8.0f;
static float mid_db   = -6.0f;
static float treble_db = 8.0f;

/* ── 压缩器状态 ── */
static float comp_env = 0.0f;

static void update_tone_stack(void)
{
    biquad_low_shelf(&tone_bass,    60.0f, 0.707f, bass_db);
    biquad_parametric(&tone_mid,   500.0f, 1.0f,   mid_db);
    biquad_high_shelf(&tone_treble, 2400.0f, 0.707f, treble_db);
}

static void amp_init(Effect *self)
{
    (void)self;

    tone_bass.x1 = tone_bass.x2 = tone_bass.y1 = tone_bass.y2 = 0.0f;
    tone_mid.x1 = tone_mid.x2 = tone_mid.y1 = tone_mid.y2 = 0.0f;
    tone_treble.x1 = tone_treble.x2 = tone_treble.y1 = tone_treble.y2 = 0.0f;

    update_tone_stack();
}

static void amp_process(Effect *self, float *in, float *out, uint16_t len)
{
    (void)self;
    float g = drive * DRIVE_MAX + 1.0f;

    for (uint16_t i = 0; i < len; i++) {
        float s = in[i];

        /* 包络检测: 硬扫弦时自动压缩 (fast attack, slow release) */
        float abs_s = s > 0.0f ? s : -s;
        if (abs_s > comp_env)
            comp_env += 0.01f * (abs_s - comp_env);   /* attack ~3ms */
        else
            comp_env += 0.0002f * (abs_s - comp_env);  /* release ~150ms */

        /* 软压缩: 信号 > 0.25 时降低增益 */
        if (comp_env > 0.25f) {
            float comp = 0.25f / comp_env;
            if (comp < 0.15f) comp = 0.15f;
            s *= comp;
        }

        /* 增益 → 推入过载区 */
        s *= g;

        /* 不对称 tanh — 偶次+奇次谐波 */
        s = tanhf(s) + 0.3f * tanhf(s / 1.5f - 0.15f);

        /* Tone Stack: 低搁架 → 中参量 → 高搁架 */
        s = biquad_tick(&tone_bass, s);
        s = biquad_tick(&tone_mid, s);
        s = biquad_tick(&tone_treble, s);

        /* 音量 + 软限幅 */
        s *= volume;
        if (s >  1.0f) s =  1.0f;
        if (s < -1.0f) s = -1.0f;

        out[i] = s;
    }
}

static void amp_set_param(Effect *self, uint8_t param_id, float value)
{
    (void)self;
    switch (param_id) {
        case 0: drive      = value; break;
        case 1: volume     = value; break;
        case 2: bass_db    = value; update_tone_stack(); break;
        case 3: mid_db     = value; update_tone_stack(); break;
        case 4: treble_db  = value; update_tone_stack(); break;
    }
}

Effect amp_sim_effect = {
    .name      = "AmpSim",
    .bypassed  = 1,
    .process   = amp_process,
    .set_param = amp_set_param,
    .init      = amp_init,
    .destroy   = 0,
    .data      = 0
};
