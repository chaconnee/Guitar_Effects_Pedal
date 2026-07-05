#include "amp_sim.h"
#include <math.h>

/*
 * AmpSim — 电子管前级过载模拟
 *
 * 信号链: 包络压缩 → 增益 → 非对称饱和 → 音量
 *
 * 参数 (set_param):
 *   id=0: drive  0.0~1.0 (增益量, 越大过载越多)
 *   id=1: volume 0.0~1.0 (输出音量)
 */

#define DRIVE_MAX 25.0f

/* ── 用户参数 ── */
static float drive  = 0.8f;
static float volume = 0.25f;

/* ── 压缩器状态 ── */
static float comp_env = 0.0f;

static void amp_init(Effect *self)
{
    (void)self;
    comp_env = 0.0f;
}

static void amp_process(Effect *self, float *in, float *out, uint16_t len)
{
    (void)self;
    float g = drive * DRIVE_MAX + 1.0f;

    for (uint16_t i = 0; i < len; i++) {
        float s = in[i];

        /* 包络检测: 硬扫弦时自动压缩 */
        float abs_s = s > 0.0f ? s : -s;
        if (abs_s > comp_env)
            comp_env += 0.01f * (abs_s - comp_env);
        else
            comp_env += 0.0002f * (abs_s - comp_env);

        if (comp_env > 0.25f) {
            float comp = 0.25f / comp_env;
            if (comp < 0.15f) comp = 0.15f;
            s *= comp;
        }

        /* 增益 → 推入过载区 */
        s *= g;

        /* 双 tanh 非对称饱和 — 产生偶次+奇次谐波 */
        s = tanhf(s) + 0.3f * tanhf(s / 1.5f - 0.15f);

        /* 音量 + 限幅 */
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
        case 0: drive  = value; break;
        case 1: volume = value; break;
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
