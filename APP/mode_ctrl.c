#include "mode_ctrl.h"
#include "effects/amp_sim/amp_sim.h"
#include "effects/cab_sim/cab_sim.h"
#include "effects/reverb/reverb.h"
#include "main.h"
#include <math.h>

volatile float g_master_volume = 1.0f;

/* ── saved parameters ── */
static float  amp_drive_val = 0.0f;
static float  rv_decay_val  = 0.95f;
static float  rv_mix_val    = 0.5f;
static float  rv_tone_val   = 0.5f;
static uint8_t cab_ir_idx   = 0;

typedef enum {
    ST_MAIN,
    ST_AMP,
    ST_CAB,
    ST_RVB
} State_t;

static State_t   state       = ST_MAIN;
static uint8_t   subpage     = 0;
static uint32_t  led_timer   = 0;
static uint8_t   led_on      = 0;
static uint8_t   chain_pos   = 0;
static uint8_t   main_blink  = 0;
static uint32_t  main_blink_timer = 0;

/* ── LED ── */
static void LED_Update(void)
{
    uint32_t now = HAL_GetTick();
    uint32_t period;

    if (state == ST_MAIN)
    {
        if (main_blink > 0)
        {
            if (now - main_blink_timer >= 200)
            {
                main_blink_timer = now;
                main_blink--;
                led_on ^= 1;
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, led_on ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }
            return;
        }
        bool any = !amp_sim_effect.bypassed || !cab_sim_effect.bypassed || !reverb_effect.bypassed;
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, any ? GPIO_PIN_RESET : GPIO_PIN_SET);
        led_on = any ? 1 : 0;
        return;
    }

    period = (state == ST_AMP ? 1000 : state == ST_CAB ? 600 : 350);
    if (now - led_timer >= period / 2)
    {
        led_timer = now;
        led_on ^= 1;
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, led_on ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
}

/* ── button rising-edge ── */
static bool Btn_Rise(GPIO_PinState *last, GPIO_PinState now)
{
    if (*last == GPIO_PIN_RESET && now == GPIO_PIN_SET)
    { *last = now; return true; }
    *last = now;
    return false;
}

/* ── enter / exit ── */
static void EnterState(State_t s, uint8_t sub)
{
    state = s; subpage = sub;
}

static void ExitState(void)
{
    amp_sim_effect.set_param(&amp_sim_effect, 0, amp_drive_val);
    reverb_effect.set_param(&reverb_effect, 0, rv_decay_val);
    reverb_effect.set_param(&reverb_effect, 1, rv_mix_val);
    reverb_effect.set_param(&reverb_effect, 2, rv_tone_val);
    state = ST_MAIN;
}

/* ── init ── */
void ModeCtrl_Init(void)
{
    amp_sim_effect.set_param(&amp_sim_effect, 0, amp_drive_val);
    reverb_effect.set_param(&reverb_effect, 0, rv_decay_val);
    reverb_effect.set_param(&reverb_effect, 1, rv_mix_val);
    reverb_effect.set_param(&reverb_effect, 2, rv_tone_val);
    cab_current_ir = cab_ir_idx;
}

/* ── poll ── */
void ModeCtrl_Poll(int16_t enc_delta)
{
    static GPIO_PinState pb0_last = GPIO_PIN_SET;
    static GPIO_PinState pb1_last = GPIO_PIN_SET;
    static GPIO_PinState pa1_last = GPIO_PIN_SET;
    static GPIO_PinState pa7_last = GPIO_PIN_SET;
    static uint32_t debounce = 0;

    /* 1. encoder delta → parameter */
    float step = (float)enc_delta * 0.005f;
    float val;

    switch (state)
    {
    case ST_MAIN:
    case ST_CAB:
        val = g_master_volume + step;
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;
        g_master_volume = val;
        break;
    case ST_AMP:
        val = amp_drive_val + step;
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;
        amp_drive_val = val;
        amp_sim_effect.set_param(&amp_sim_effect, 0, val);
        break;
    case ST_RVB:
        if (subpage == 0) { val = rv_decay_val + step; if (val < 0.0f) val = 0.0f; if (val > 1.0f) val = 1.0f; rv_decay_val = val; reverb_effect.set_param(&reverb_effect, 0, val); }
        if (subpage == 1) { val = rv_mix_val   + step; if (val < 0.0f) val = 0.0f; if (val > 1.0f) val = 1.0f; rv_mix_val   = val; reverb_effect.set_param(&reverb_effect, 1, val); }
        if (subpage == 2) { val = rv_tone_val  + step; if (val < 0.0f) val = 0.0f; if (val > 1.0f) val = 1.0f; rv_tone_val  = val; reverb_effect.set_param(&reverb_effect, 2, val); }
        break;
    }

    /* 2. LED every frame */
    LED_Update();

    /* 3. buttons every 100ms */
    if (HAL_GetTick() - debounce < 100) return;
    debounce = HAL_GetTick();

    GPIO_PinState pb0 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14);
    GPIO_PinState pb1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);
    GPIO_PinState pa1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
    GPIO_PinState pa7 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);

    /* ── PB1: bypass toggle (MAIN=highlighted, mode=specific) ── */
    if (Btn_Rise(&pb1_last, pb1))
    {
        if (state == ST_MAIN)
        {
            if (chain_pos == 0)      amp_sim_effect.bypassed = !amp_sim_effect.bypassed;
            else if (chain_pos == 1) cab_sim_effect.bypassed = !cab_sim_effect.bypassed;
            else                     reverb_effect.bypassed  = !reverb_effect.bypassed;
        }
        else if (state == ST_AMP)
            amp_sim_effect.bypassed  = !amp_sim_effect.bypassed;
        else if (state == ST_CAB)
            cab_sim_effect.bypassed  = !cab_sim_effect.bypassed;
        else if (state == ST_RVB)
            reverb_effect.bypassed   = !reverb_effect.bypassed;
    }

    /* ── PA7: EXIT (parameter modes → MAIN) ── */
    if (Btn_Rise(&pa7_last, pa7))
    {
        if (state != ST_MAIN) ExitState();
    }

    /* ── PA1: cycle effect chain highlight (MAIN only) ── */
    if (Btn_Rise(&pa1_last, pa1))
    {
        if (state == ST_MAIN)
        {
            chain_pos = (chain_pos + 1) % 3;
            main_blink = (chain_pos + 1) * 2;
            main_blink_timer = HAL_GetTick();
            led_on = 0;
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        }
    }

    /* ── PB0: enter / cycle ── */
    if (Btn_Rise(&pb0_last, pb0))
    {
        switch (state)
        {
        case ST_MAIN:
            if (chain_pos == 0)      EnterState(ST_AMP, 0);
            else if (chain_pos == 1) EnterState(ST_CAB, 0);
            else                     EnterState(ST_RVB, 0);
            break;
        case ST_AMP:
            break;
        case ST_CAB:
            cab_ir_idx = (cab_ir_idx + 1) % IR_COUNT;
            CabSim_SelectIR(cab_ir_idx);
            break;
        case ST_RVB:
            subpage = (subpage + 1) % 3;
            break;
        }
    }
}
