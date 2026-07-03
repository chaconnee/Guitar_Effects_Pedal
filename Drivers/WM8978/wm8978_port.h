/**
  ******************************************************************************
  * @file    wm8978_port.h
  * @brief   WM8978 I2C port layer
  ******************************************************************************
  */

#ifndef WM8978_PORT_H
#define WM8978_PORT_H

#include "wm8978.h"

int32_t WM8978_PORT_Init(void);

/* Test I2C communication: read chip ID. 0=OK, <0=fail */
int32_t WM8978_PORT_Test(void);

/* Re-enable ADC after MCLK is stable (must be called after I2S starts) */
void WM8978_PORT_EnableADC(void);

WM8978_Object_t* WM8978_PORT_GetObj(void);

#endif /* WM8978_PORT_H */
