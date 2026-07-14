#include "reverb.h"
#include <math.h>

/*
 * Reverb — Schroeder 混响
 *
 * 信号链: 输入 → [4并联反馈梳状(IIR,带阻尼)] → ÷4 → [3串联全通] → 干湿混合 → 输出
 *         前馈梳状(FIR): H(z)=1-g·z^{-N}   (陷波, 零点在单位圆)
 *         反馈梳状(IIR): H(z)=z^{-M}/(1-g·z^{-M})  (峰值, 极点在单位圆附近)
 *
 * 参数 (set_param):
 *   id=0: decay  0.0~1.0 (混响尾长, 越大越长)
 *   id=1: mix    0.0~1.0 (干湿比, 越大越湿)
 *   id=2: tone   0.0~1.0 (亮度, 越大越亮 → LPF α=0.1~0.95)
 */

/* ── 延迟长度 (32kHz 采样率, Schroeder 原始值 ×1.28) ── */
#define CB0_LEN  2214
#define CB1_LEN  1912
#define CB2_LEN  2588
#define CB3_LEN  2875
#define AP0_LEN   320
#define AP1_LEN   108
#define AP2_LEN    31

/* ── 延迟缓冲区 ── */
static float cb0_buf[CB0_LEN];
static float cb1_buf[CB1_LEN];
static float cb2_buf[CB2_LEN];
static float cb3_buf[CB3_LEN];
static float ap0_buf[AP0_LEN];
static float ap1_buf[AP1_LEN];
static float ap2_buf[AP2_LEN];

/* ── 写指针 ── */
static int cb0_ptr, cb1_ptr, cb2_ptr, cb3_ptr;
static int ap0_ptr, ap1_ptr, ap2_ptr;

/* ── LPF 状态（一阶低通, prev 存储上一输出） ── */
static float cb0_prev, cb1_prev, cb2_prev, cb3_prev;

/* ── 用户参数 ── */
static float decay = 0.95f;
static float mix   = 0.55f;
static float tone  = 0.15f;

/* ── 参数映射 ── */

/* Decay 0.0~1.0 → 反馈增益 0.5~0.92 */
static float decay_to_feedback(float d)
{
    return 0.5f + d * 0.42f;
}

/* Tone 0.0~1.0 → LPF α 0.1~0.95 (α=0.1 强低通/暗, α=0.95 近全通/亮) */
static float tone_to_lpf_alpha(float t)
{
    return 0.1f + t * 0.85f;
}

/*
 * 反馈梳状滤波器 (Feedback Comb, IIR)
 *   H(z) = z^{-M} / (1 - fb·H_lp(z)·z^{-M})
 *   与 H(z) = 1 - g·z^{-N} (前馈/FIR/陷波) 不同:
 *   前馈 = 零点在单位圆上, 产生无限深陷波, 无反馈
 *   反馈 = 极点在单位圆附近, 产生等距峰值, 有反馈回路
 */

static inline float comb_process(float *buf, int *ptr, int len,
                                 float feedback, float lpf_alpha, float *prev,
                                 float input)
{
    float readback = buf[*ptr];
    *prev += lpf_alpha * (readback - *prev);   /* 一阶低通 */
    buf[*ptr] = input + (*prev) * feedback;
    (*ptr)++;
    if (*ptr >= len) *ptr = 0;
    return readback;
}

/* ── 全通滤波器 ── */

static inline float allpass_process(float *buf, int *ptr, int len,
                                    float gain, float input)
{
    float readback = buf[*ptr];
    readback += (-gain) * input;
    buf[*ptr] = input + readback * gain;
    (*ptr)++;
    if (*ptr >= len) *ptr = 0;
    return readback;
}

/* ── Effect 接口实现 ── */

static void reverb_init(Effect *self)
{
    (void)self;
    cb0_ptr = cb1_ptr = cb2_ptr = cb3_ptr = 0;
    ap0_ptr = ap1_ptr = ap2_ptr = 0;
    cb0_prev = cb1_prev = cb2_prev = cb3_prev = 0.0f;

    for (int i = 0; i < CB0_LEN; i++) cb0_buf[i] = 0.0f;
    for (int i = 0; i < CB1_LEN; i++) cb1_buf[i] = 0.0f;
    for (int i = 0; i < CB2_LEN; i++) cb2_buf[i] = 0.0f;
    for (int i = 0; i < CB3_LEN; i++) cb3_buf[i] = 0.0f;
    for (int i = 0; i < AP0_LEN; i++) ap0_buf[i] = 0.0f;
    for (int i = 0; i < AP1_LEN; i++) ap1_buf[i] = 0.0f;
    for (int i = 0; i < AP2_LEN; i++) ap2_buf[i] = 0.0f;
}

static void reverb_process(Effect *self, float *in, float *out, uint16_t len)
{
    (void)self;
    float fb    = decay_to_feedback(decay);
    float alpha = tone_to_lpf_alpha(tone);
    float ap_g = 0.7f;

    for (uint16_t i = 0; i < len; i++) {
        float s = in[i];

        /* 4 并联梳状滤波器 */
        float wet = (comb_process(cb0_buf, &cb0_ptr, CB0_LEN, fb, alpha, &cb0_prev, s)
                   + comb_process(cb1_buf, &cb1_ptr, CB1_LEN, fb, alpha, &cb1_prev, s)
                   + comb_process(cb2_buf, &cb2_ptr, CB2_LEN, fb, alpha, &cb2_prev, s)
                   + comb_process(cb3_buf, &cb3_ptr, CB3_LEN, fb, alpha, &cb3_prev, s)) * 0.25f;

        /* 3 串联全通滤波器 */
        wet = allpass_process(ap0_buf, &ap0_ptr, AP0_LEN, ap_g, wet);
        wet = allpass_process(ap1_buf, &ap1_ptr, AP1_LEN, ap_g, wet);
        wet = allpass_process(ap2_buf, &ap2_ptr, AP2_LEN, ap_g, wet);

        /* 干湿混合 */
        out[i] = (1.0f - mix) * s + mix * wet;
    }
}

static void reverb_set_param(Effect *self, uint8_t param_id, float value)
{
    (void)self;
    switch (param_id) {
        case 0: decay = value; break;
        case 1: mix   = value; break;
        case 2: tone  = value; break;
    }
}

Effect reverb_effect = {
    .name      = "Reverb",
    .bypassed  = 0,
    .process   = reverb_process,
    .set_param = reverb_set_param,
    .init      = reverb_init,
    .destroy   = 0,
    .data      = 0
};
