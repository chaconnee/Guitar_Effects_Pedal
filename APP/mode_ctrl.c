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
void ModeCtrl_Poll(void)
{
    static GPIO_PinState btn1_last = GPIO_PIN_SET;
    static GPIO_PinState btn2_last = GPIO_PIN_SET;
    static GPIO_PinState btn3_last = GPIO_PIN_SET;
    static uint32_t debounce = 0;

    /* 1. LED every frame */
    LED_Update();

    /* 2. buttons every 100ms */
    if (HAL_GetTick() - debounce < 100) return;
    debounce = HAL_GetTick();

    GPIO_PinState btn1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
    GPIO_PinState btn2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
    GPIO_PinState btn3 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);

    /* ── btn3: EXIT (parameter modes → MAIN) ── */
    if (Btn_Rise(&btn3_last, btn3))
    {
        if (state != ST_MAIN) ExitState();
    }

    /* ── btn1: cycle effect chain highlight (MAIN only) ── */
    if (Btn_Rise(&btn1_last, btn1))
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

    /* ── btn2: enter / cycle ── */
    if (Btn_Rise(&btn2_last, btn2))
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
