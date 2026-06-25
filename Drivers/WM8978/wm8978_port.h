/**
  ******************************************************************************
  * @file    wm8978_port.h
  * @brief   WM8978 I2C port layer - bridges WM8978 driver to STM32 HAL I2C
  ******************************************************************************
  */

#ifndef WM8978_PORT_H
#define WM8978_PORT_H

#include "wm8978.h"

/**
  * @brief Initialize the WM8978 codec with I2C bus IO and apply guitar config.
  *        Must be called after MX_I2C1_Init().
  * @retval WM8978_OK on success
  */
int32_t WM8978_PORT_Init(void);

/**
  * @brief Get pointer to the codec object (for volume/mute/EQ control).
  */
WM8978_Object_t* WM8978_PORT_GetObj(void);

#endif /* WM8978_PORT_H */
