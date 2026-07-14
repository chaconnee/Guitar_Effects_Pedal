#include "encoder.h"
#include "tim.h"
#include "main.h"
#include <stdbool.h>

void Enc_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
}

int16_t Enc_ReadDelta(void)
{
    static uint16_t last  = 0;
    static bool     first = true;

    uint16_t now = (uint16_t)__HAL_TIM_GET_COUNTER(&htim2);

    if (first) { last = now; first = false; return 0; }

    int16_t delta = (int16_t)(now - last);
    last = now;
    return delta;
}
