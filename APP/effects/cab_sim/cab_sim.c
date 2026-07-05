#include "cab_sim.h"
#include "arm_math.h"
#include <string.h>
#include "main.h"

#include "effects/cab_sim/ir_AC30 brilliant+bass AT4033a stalevel_dc.h"

/*
 * Overlap-save fast convolution (BLOCK_SIZE=128, 4ms 块周期)
 * FFT_SIZE=1024, IR_SIZE=897 (覆盖 OwnHammer 99.77% 能量, 28ms)
 */

#define BLOCK_SIZE  128
#define IR_SIZE     897
#define FFT_SIZE    1024
#define FFT_SIZE_X2 (FFT_SIZE * 2)

static arm_rfft_fast_instance_f32 fft_inst;

static float ir_fft   [FFT_SIZE_X2];
static float in_fft   [FFT_SIZE_X2];
static float slide_buf[FFT_SIZE];

static void cab_init(Effect *self)
{
    (void)self;
    static float ir_tmp[FFT_SIZE];

    arm_rfft_fast_init_f32(&fft_inst, FFT_SIZE);

    /* 加载 IR: 取前 IR_SIZE 个样本 (NeuralRack 风格归一化: 峰值0.8 + 功率归一化) */
    memset(ir_tmp, 0, sizeof(ir_tmp));
    uint32_t copy_len = IR_AC30_BRILLIANT_BASS_AT4033A_STALEVEL_DC_LENGTH < IR_SIZE ? IR_AC30_BRILLIANT_BASS_AT4033A_STALEVEL_DC_LENGTH : IR_SIZE;
    memcpy(ir_tmp, ir_AC30_brilliant_bass_AT4033a_stalevel_dc, copy_len * sizeof(float));

    /* 预计算 IR 的 FFT */
    memset(ir_fft, 0, sizeof(ir_fft));
    memcpy(ir_fft, ir_tmp, FFT_SIZE * sizeof(float));
    arm_rfft_fast_f32(&fft_inst, ir_fft, ir_fft + FFT_SIZE, 0);

    memset(slide_buf, 0, sizeof(slide_buf));
    memset(in_fft, 0, sizeof(in_fft));
}

static void cab_process(Effect *self, float *in, float *out, uint16_t len)
{
    (void)self;
    (void)len;

    /* 1. 滑动窗口: 左移 BLOCK_SIZE, 空出末尾填入新数据 */
    // [ a0 a1 ... a127 │ a128 a129 ... a895 a896 ... a1023 ]
    //   └─ 最老 128 ─┘ └────── 要保留的 896 ─────┘
    // [ a128 a129 ... a895 a896 ... a1023 │ ??? ??? ... ??? ]
    //   └─────── 保留的 896 ──────┘ └─ 空出的 128 ─┘
    // [ a128 a129 ... a895 a896 ... a1023 │ b0 b1 ... b127 ]
    //   └─────── 保留的 896 ──────┘ └─ 新128采样 ─┘
    memmove(slide_buf, slide_buf + BLOCK_SIZE, (FFT_SIZE - BLOCK_SIZE) * sizeof(float));
    memcpy(slide_buf + (FFT_SIZE - BLOCK_SIZE), in, BLOCK_SIZE * sizeof(float));

    /* 2. 正向 FFT */
    memcpy(in_fft, slide_buf, FFT_SIZE * sizeof(float));
    arm_rfft_fast_f32(&fft_inst, in_fft, in_fft + FFT_SIZE, 0);

    /* 3. 频域乘法 */
    arm_cmplx_mult_cmplx_f32(
        ir_fft + FFT_SIZE,
        in_fft + FFT_SIZE,
        in_fft,
        FFT_SIZE / 2
    );

    /* 4. 逆 FFT */
    arm_rfft_fast_f32(&fft_inst, in_fft, in_fft + FFT_SIZE, 1);

    /* 5. 提取有效输出  */
    const float *ifft_out = in_fft + FFT_SIZE;

    //  IFFT 输出 (1024 采样):
    //  [0──────────────────895]  [896────────────1023]
    //    └─ 被圆周卷积混叠污染 ─┘         └─ 正确的线性卷积 ─┘
    //              丢弃                                     保留 = out[0..127]
    for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
        out[i] = ifft_out[FFT_SIZE - BLOCK_SIZE + i];
    }
}

static void cab_set_param(Effect *self, uint8_t param_id, float value)
{
    (void)self;
    (void)param_id;
    (void)value;
}

Effect cab_sim_effect = {
    .name = "CabSim",
    .bypassed = 0,
    .process = cab_process,
    .set_param = cab_set_param,
    .init = cab_init,
    .destroy = 0,
    .data = 0
};
