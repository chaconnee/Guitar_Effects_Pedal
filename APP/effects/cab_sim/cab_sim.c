#include "cab_sim.h"
#include "arm_math.h"
#include <string.h>
#include "main.h"

#include "effects/cab_sim/ir_zila_212.h"
#include "effects/cab_sim/ir_AC30 brilliant+bass AT4033a stalevel_dc.h"
#include "effects/cab_sim/YA VX30 212 BLU Mix 01.h"
#include "effects/cab_sim/YA VX15 112 BLU Mix 01.h"
#include "effects/cab_sim/YA MTCH 212 ESD H Mix 05.h"
#include "effects/cab_sim/YA PRNC 110 OX Mix 09.h"
#include "effects/cab_sim/YA VLUX 210 P10R Mix 11.h"
#include "effects/cab_sim/ir_slot5.h"
#include "effects/cab_sim/ir_slot6.h"
#include "effects/cab_sim/ir_slot7.h"
#include "effects/cab_sim/ir_slot8.h"
#include "effects/cab_sim/ir_slot9.h"
#include "effects/cab_sim/ir_slot10.h"

#define BLOCK_SIZE  128
#define IR_SIZE     897
#define FFT_SIZE    1024
#define FFT_SIZE_X2 (FFT_SIZE * 2)

static arm_rfft_fast_instance_f32 fft_inst;
static float ir_fft   [FFT_SIZE_X2];
static float in_fft   [FFT_SIZE_X2];
static float slide_buf[FFT_SIZE];

volatile uint32_t g_cab_us_max  = 0;
volatile uint32_t g_cab_us_min  = 0xFFFFFFFF;
volatile uint64_t g_cab_us_sum  = 0;
volatile uint32_t g_cab_cnt     = 0;
volatile uint32_t g_cab_us_last = 0;

uint8_t cab_current_ir = 2;

static const float *ir_tables[IR_COUNT] = { ir_zila_212, ir_AC30_brilliant_bass_AT4033a_stalevel_dc, YA_VX30_212_BLU_Mix_01, YA_VX15_112_BLU_Mix_01, YA_MTCH_212_ESD_H_Mix_05, YA_PRNC_110_OX_Mix_09, YA_VLUX_210_P10R_Mix_11, ir_slot5, ir_slot6, ir_slot7, ir_slot8, ir_slot9, ir_slot10 };
static const uint32_t ir_lengths[IR_COUNT] = { IR_ZILA_212_LENGTH, IR_AC30_BRILLIANT_BASS_AT4033A_STALEVEL_DC_LENGTH, YA_VX30_212_BLU_MIX_01_LENGTH, YA_VX15_112_BLU_MIX_01_LENGTH, YA_MTCH_212_ESD_H_MIX_05_LENGTH, YA_PRNC_110_OX_MIX_09_LENGTH,YA_VLUX_210_P10R_MIX_11_LENGTH, IR_SLOT5_LENGTH, IR_SLOT6_LENGTH, IR_SLOT7_LENGTH, IR_SLOT8_LENGTH, IR_SLOT9_LENGTH, IR_SLOT10_LENGTH };

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

    static uint8_t dwt_ready = 0;
    if (!dwt_ready)
    {
        CoreDebug->DEMCR |= (1 << 24);
        DWT->CYCCNT = 0;
        DWT->CTRL  |= (1 << 0);
        dwt_ready = 1;
    }
    uint32_t t0 = DWT->CYCCNT;

    memmove(slide_buf, slide_buf + BLOCK_SIZE, (FFT_SIZE - BLOCK_SIZE) * sizeof(float));
    memcpy(slide_buf + (FFT_SIZE - BLOCK_SIZE), in, BLOCK_SIZE * sizeof(float));
    memcpy(in_fft, slide_buf, FFT_SIZE * sizeof(float));
    arm_rfft_fast_f32(&fft_inst, in_fft, in_fft + FFT_SIZE, 0);
    arm_cmplx_mult_cmplx_f32(ir_fft + FFT_SIZE, in_fft + FFT_SIZE, in_fft, FFT_SIZE / 2);
    arm_rfft_fast_f32(&fft_inst, in_fft, in_fft + FFT_SIZE, 1);
    const float *ifft_out = in_fft + FFT_SIZE;
    for (uint16_t i = 0; i < BLOCK_SIZE; i++)
        out[i] = ifft_out[FFT_SIZE - BLOCK_SIZE + i];

    uint32_t us = (DWT->CYCCNT - t0) / 96;
    g_cab_us_last = us;
    if (us > g_cab_us_max) g_cab_us_max = us;
    if (us < g_cab_us_min) g_cab_us_min = us;
    g_cab_us_sum += us;
    g_cab_cnt++;
}

static void cab_set_param(Effect *self, uint8_t param_id, float value)
{ (void)self; (void)param_id; (void)value; }

Effect cab_sim_effect = {
    .name = "CabSim", .bypassed = 0, .process = cab_process,
    .set_param = cab_set_param, .init = cab_init, .destroy = 0, .data = 0
};
