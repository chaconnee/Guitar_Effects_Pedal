#include "pot.h"
#include "adc.h"
#include "main.h"

void Pot_Init(void) { }

float Pot_Read(void)
{
    static float smoothed = 0.5f;

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)
    {
        float raw = (float)HAL_ADC_GetValue(&hadc1) / 4095.0f;
        smoothed += 0.1f * (raw - smoothed);
    }
    HAL_ADC_Stop(&hadc1);

    if (smoothed < 0.0f) smoothed = 0.0f;
    if (smoothed > 1.0f) smoothed = 1.0f;
    return smoothed;
}
