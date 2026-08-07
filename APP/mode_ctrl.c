#include "mode_ctrl.h"
#include "effects/amp_sim/amp_sim.h"
#include "effects/cab_sim/cab_sim.h"
#include "effects/reverb/reverb.h"
#include "tft.h"
#include "main.h"
#include <math.h>
#include <string.h>

volatile float g_master_volume = 1.0f;

/* ── UI 颜色 (屏为反相面板: 发 0x0000 得白、发 0xFFFF 得黑) ── */
#define CLR_BG       0xFFFF   /* 物理黑底 */
#define CLR_TEXT     0x0000   /* 物理白字 */
#define CLR_SEL_BG   0x01AF   /* 选中行天蓝 */
#define CLR_SEL_FG   0x0000   /* 选中行白字 */
#define CLR_ON       0x0000   /* 直通白字 */
#define CLR_BYP      0x7BEF   /* 旁通灰字 */
#define CLR_BAR_BG   0x7BEF   /* 音量条底深灰 */
#define CLR_BAR      0x0000   /* 音量条白 */
#define CLR_LINE     0xFFE0   /* 分隔线红 */
#define CLR_HINT     0x7BEF   /* 提示灰 */

/* ── 状态 ── */
typedef enum { ST_MAIN, ST_VOL, ST_IR } State_t;
typedef enum { ITEM_AMP, ITEM_CAB, ITEM_RVB, ITEM_VOL, ITEM_IR, ITEM_NUM } MainItem_t;

static const char *ir_names[IR_COUNT] = { "ZILA212", "AC30BB ", "VX30212 ", "VX15112 ", "MTCH212 ", "PRNC110 ", "VLUX210 ", "SLOT5  ", "SLOT6  ", "SLOT7  ", "SLOT8  ", "SLOT9  ", "SLOT10 " };

static State_t     state    = ST_MAIN;
static MainItem_t  item     = ITEM_CAB;
static uint8_t     vol_pct  = 100;   /* 主音量 % */
static uint8_t     cab_ir_idx = 2;   /* 当前 IR 索引 */

static uint32_t  led_timer    = 0;
static uint8_t   led_on       = 0;
static uint8_t   main_blink   = 0;
static uint32_t  main_blink_timer = 0;

/* ── 数值转字符串 ── */
static void Utoa(uint16_t v, char *buf)
{
    char tmp[6];
    int n = 0;
    do { tmp[n++] = (char)('0' + v % 10); v /= 10; } while (v);
    for (int i = 0; i < n; i++) buf[i] = tmp[n - 1 - i];
    buf[n] = '\0';
}

/* ── LED ── */
static void LED_Update(void)
{
    uint32_t now = HAL_GetTick();

    if (state == ST_MAIN)
    {
        if (main_blink > 0)
        {
            if (now - main_blink_timer >= 150)
            {
                main_blink_timer = now;
                main_blink--;
                led_on ^= 1;
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, led_on ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }
            return;
        }
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        led_on = 0;
        return;
    }

    if (now - led_timer >= 250)
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

/* ── TFT 渲染 ── */
static void MenuRow(uint8_t row, const char *label, const char *value,
                    uint16_t valColor, bool selected)
{
    uint16_t y = 10 + row * 10;

    if (selected)
    {
        TFT_FillRect(0, y, 160, 10, CLR_SEL_BG);
        TFT_DrawStrEx(2, y, ">", CLR_SEL_FG, CLR_SEL_BG);
        TFT_DrawStrEx(12, y, label, CLR_SEL_FG, CLR_SEL_BG);
        TFT_DrawStrEx(160 - 2 - (uint16_t)strlen(value) * 6, y, value, CLR_SEL_FG, CLR_SEL_BG);
    }
    else
    {
        TFT_DrawStr(12, y, label, CLR_TEXT);
        TFT_DrawStr(160 - 2 - (uint16_t)strlen(value) * 6, y, value, valColor);
    }
}

static void DrawBar(float frac)
{
    if (frac < 0.0f) frac = 0.0f;
    if (frac > 1.0f) frac = 1.0f;
    TFT_FillRect(4, 0, 152, 5, CLR_BAR_BG);
    if (frac > 0.02f)
        TFT_FillRect(4, 0, (uint16_t)(152.0f * frac), 5, CLR_BAR);
}

static void Render(void)
{
    TFT_FillScreen(CLR_BG);

    /* 音量条放底部 */
    DrawBar(g_master_volume);

    char vbuf[6];
    Utoa(vol_pct, vbuf);

    MenuRow(0, "AMP    ", amp_sim_effect.bypassed ? "BYP" : "ON",
            amp_sim_effect.bypassed ? CLR_BYP : CLR_ON, state == ST_MAIN && item == ITEM_AMP);
    MenuRow(1, "CAB    ", cab_sim_effect.bypassed ? "BYP" : "ON",
            cab_sim_effect.bypassed ? CLR_BYP : CLR_ON, state == ST_MAIN && item == ITEM_CAB);
    MenuRow(2, "REVERB ", reverb_effect.bypassed ? "BYP" : "ON",
            reverb_effect.bypassed ? CLR_BYP : CLR_ON, state == ST_MAIN && item == ITEM_RVB);
    MenuRow(3, "VOLUME ", vbuf, CLR_TEXT, (state == ST_VOL) || (state == ST_MAIN && item == ITEM_VOL));

    char ibuf[24], idxbuf[8];
    Utoa(cab_current_ir + 1, idxbuf);
    strcpy(ibuf, "#");
    strcat(ibuf, idxbuf);
    strcat(ibuf, " ");
    strcat(ibuf, ir_names[cab_current_ir]);
    MenuRow(4, "IR     ", ibuf, CLR_TEXT, (state == ST_IR) || (state == ST_MAIN && item == ITEM_IR));

    switch (state)
    {
    case ST_MAIN: TFT_DrawStr(4, 68, "K1:sel K2:byp K3:ok", CLR_HINT); break;
    case ST_VOL:  TFT_DrawStr(4, 68, "K1:-  K2:+  K3:back", CLR_HINT); break;
    case ST_IR:   TFT_DrawStr(4, 68, "K1:prev K2:next K3:back", CLR_HINT); break;
    }
}

static bool ui_dirty = true;

/* ── init ── */
void ModeCtrl_Init(void)
{
    amp_sim_effect.bypassed = 1;          /* ampsim 暂不使用 */
    cab_ir_idx = cab_current_ir;
    g_master_volume = (float)vol_pct / 100.0f;
    ui_dirty = true;
}

/* ── poll ── */
void ModeCtrl_Poll(void)
{
    static GPIO_PinState btn1_last = GPIO_PIN_SET;
    static GPIO_PinState btn2_last = GPIO_PIN_SET;
    static GPIO_PinState btn3_last = GPIO_PIN_SET;
    static uint32_t debounce = 0;
    static uint32_t back_lock = 0;   /* 退出后短时屏蔽重入，避开抖动 */

    LED_Update();

    if (ui_dirty)
    {
        Render();
        ui_dirty = false;
    }

    if (HAL_GetTick() - debounce < 100) return;
    debounce = HAL_GetTick();

    GPIO_PinState btn1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
    GPIO_PinState btn2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
    GPIO_PinState btn3 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);

    /* ── btn3: 进入选中子页 / 返回主页 ── */
    if (Btn_Rise(&btn3_last, btn3))
    {
        if (state == ST_MAIN)
        {
            if (HAL_GetTick() < back_lock)
            {
                /* 刚从子页退出，短暂忽略避免抖动重入 */
            }
            else if (item == ITEM_VOL) state = ST_VOL;
            else if (item == ITEM_IR) state = ST_IR;
            ui_dirty = true;
        }
        else
        {
            state = ST_MAIN;
            back_lock = HAL_GetTick() + 300;
            ui_dirty = true;
        }
    }

    /* ── btn1 ── */
    if (Btn_Rise(&btn1_last, btn1))
    {
        switch (state)
        {
        case ST_MAIN:
            item = (MainItem_t)((item + 1) % ITEM_NUM);
            main_blink = (item + 1) * 2;
            main_blink_timer = HAL_GetTick();
            led_on = 0;
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
            ui_dirty = true;
            break;
        case ST_VOL:
            if (vol_pct > 0) { vol_pct -= 5; g_master_volume = (float)vol_pct / 100.0f; ui_dirty = true; }
            break;
        case ST_IR:
            cab_ir_idx = (cab_ir_idx + IR_COUNT - 1) % IR_COUNT;
            CabSim_SelectIR(cab_ir_idx);
            ui_dirty = true;
            break;
        }
    }

    /* ── btn2 ── */
    if (Btn_Rise(&btn2_last, btn2))
    {
        switch (state)
        {
        case ST_MAIN:
            if (item == ITEM_AMP) { amp_sim_effect.bypassed = !amp_sim_effect.bypassed; ui_dirty = true; }
            else if (item == ITEM_CAB) { cab_sim_effect.bypassed = !cab_sim_effect.bypassed; ui_dirty = true; }
            else if (item == ITEM_RVB) { reverb_effect.bypassed = !reverb_effect.bypassed; ui_dirty = true; }
            break;
        case ST_VOL:
            if (vol_pct < 100) { vol_pct += 5; g_master_volume = (float)vol_pct / 100.0f; ui_dirty = true; }
            break;
        case ST_IR:
            cab_ir_idx = (cab_ir_idx + 1) % IR_COUNT;
            CabSim_SelectIR(cab_ir_idx);
            ui_dirty = true;
            break;
        }
    }
}
