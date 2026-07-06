#include "cab_sim.h"
#include "arm_math.h"
#include <string.h>
#include "main.h"

#include "effects/cab_sim/ir_zila_212.h"
#include "effects/cab_sim/ir_ac30.h"
#include "effects/cab_sim/ir_OwnHammer_412.h"

#define BLOCK_SIZE  128
#define IR_SIZE     897
#define FFT_SIZE    1024
#define FFT_SIZE_X2 (FFT_SIZE * 2)

static arm_rfft_fast_instance_f32 fft_inst;
static float ir_fft   [FFT_SIZE_X2];
static float in_fft   [FFT_SIZE_X2];
static float slide_buf[FFT_SIZE];

uint8_t cab_current_ir = 0;

static const float *ir_tables[IR_COUNT] = { ir_zila_212, ir_ac30, ir_OwnHammer_412 };
static const uint32_t ir_lengths[IR_COUNT] = { IR_ZILA_212_LENGTH, IR_AC30_LENGTH, IR_OWNHAMMER_412_LENGTH };

void CabSim_SelectIR(uint8_t idx)
{
    if (idx >= IR_COUNT) return;
    const float *ir_table = ir_tables[idx];
    uint32_t ir_len = ir_lengths[idx];

    static float ir_tmp[FFT_SIZE];
    memset(ir_tmp, 0, sizeof(ir_tmp));
    uint32_t copy_len = ir_len < IR_SIZE ? ir_len : IR_SIZE;
    memcpy(ir_tmp, ir_table, copy_len * sizeof(float));

    memset(ir_fft, 0, sizeof(ir_fft));
    memcpy(ir_fft, ir_tmp, FFT_SIZE * sizeof(float));
    arm_rfft_fast_f32(&fft_inst, ir_fft, ir_fft + FFT_SIZE, 0);
    cab_current_ir = idx;
}

static void cab_init(Effect *self)
{
    (void)self;
    arm_rfft_fast_init_f32(&fft_inst, FFT_SIZE);
    memset(slide_buf, 0, sizeof(slide_buf));
    memset(in_fft, 0, sizeof(in_fft));
    CabSim_SelectIR(cab_current_ir);
}

static void cab_process(Effect *self, float *in, float *out, uint16_t len)
{
    (void)self; (void)len;
    memmove(slide_buf, slide_buf + BLOCK_SIZE, (FFT_SIZE - BLOCK_SIZE) * sizeof(float));
    memcpy(slide_buf + (FFT_SIZE - BLOCK_SIZE), in, BLOCK_SIZE * sizeof(float));
    memcpy(in_fft, slide_buf, FFT_SIZE * sizeof(float));
    arm_rfft_fast_f32(&fft_inst, in_fft, in_fft + FFT_SIZE, 0);
    arm_cmplx_mult_cmplx_f32(ir_fft + FFT_SIZE, in_fft + FFT_SIZE, in_fft, FFT_SIZE / 2);
    arm_rfft_fast_f32(&fft_inst, in_fft, in_fft + FFT_SIZE, 1);
    const float *ifft_out = in_fft + FFT_SIZE;
    for (uint16_t i = 0; i < BLOCK_SIZE; i++)
        out[i] = ifft_out[FFT_SIZE - BLOCK_SIZE + i];
}

static void cab_set_param(Effect *self, uint8_t param_id, float value)
{ (void)self; (void)param_id; (void)value; }

Effect cab_sim_effect = {
    .name = "CabSim", .bypassed = 0, .process = cab_process,
    .set_param = cab_set_param, .init = cab_init, .destroy = 0, .data = 0
};
