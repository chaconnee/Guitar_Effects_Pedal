/**
  ******************************************************************************
  * @file    wm8978_port.c
  * @brief   WM8978 I2C port layer - bridges WM8978 driver to STM32 HAL I2C
  ******************************************************************************
  */

#include "wm8978_port.h"
#include "i2c.h"

static WM8978_Object_t wm8978_obj;

static int32_t WM8978_IO_Init(void)
{
  /* I2C already initialized in main, nothing extra needed */
  return WM8978_OK;
}

static int32_t WM8978_IO_DeInit(void)
{
  return WM8978_OK;
}

static int32_t WM8978_IO_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  (void)Reg;
  /* WM8978 I2C address is 7-bit: 0x1A */
  if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(Addr << 1), pData, Length, 100) != HAL_OK)
  {
    return WM8978_ERROR;
  }
  return WM8978_OK;
}

static int32_t WM8978_IO_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  (void)Reg;
  /* WM8978 registers are write-only; this should not be called */
  (void)Addr;
  (void)pData;
  (void)Length;
  return WM8978_ERROR;
}

static int32_t WM8978_IO_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}

int32_t WM8978_PORT_Init(void)
{
  WM8978_IO_t io;

  io.Init     = WM8978_IO_Init;
  io.DeInit   = WM8978_IO_DeInit;
  io.Address  = WM8978_I2C_ADDR_DEFAULT;  /* 0x1A */
  io.WriteReg = WM8978_IO_WriteReg;
  io.ReadReg  = WM8978_IO_ReadReg;
  io.GetTick  = WM8978_IO_GetTick;

  /* Register bus IO */
  if (WM8978_RegisterBusIO(&wm8978_obj, &io) != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  /* Apply guitar-specific initialization */
  if (WM8978_Init_Guitar(&wm8978_obj) != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  return WM8978_OK;
}

WM8978_Object_t* WM8978_PORT_GetObj(void)
{
  return &wm8978_obj;
}
