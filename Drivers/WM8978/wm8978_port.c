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
  return WM8978_OK;
}

static int32_t WM8978_IO_DeInit(void)
{
  return WM8978_OK;
}

/* WM8978_WriteRegister 已将数据编码为 WM8978 9-bit 协议, IO 层直接透传 */
static int32_t WM8978_IO_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  (void)Reg;
  if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(Addr << 1), pData, Length, 100) != HAL_OK)
    return WM8978_ERROR;
  return WM8978_OK;
}

static int32_t WM8978_IO_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(Addr << 1), Reg,
                       I2C_MEMADD_SIZE_8BIT, pData, Length, 100) != HAL_OK)
    return WM8978_ERROR;
  return WM8978_OK;
}

static int32_t WM8978_IO_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}

/* 直接写寄存器: 编码 9-bit 协议后通过 IO 层发送 */
static void port_write_reg(uint8_t reg, uint16_t val)
{
  uint8_t buf[2];
  buf[0] = (uint8_t)(((reg << 1) & 0xFE) | ((val >> 8) & 0x01));
  buf[1] = (uint8_t)(val & 0xFF);
  wm8978_obj.IO.WriteReg(wm8978_obj.IO.Address, 0, buf, 2);
}

int32_t WM8978_PORT_Init(void)
{
  WM8978_IO_t io = {0};

  io.Init     = WM8978_IO_Init;
  io.DeInit   = WM8978_IO_DeInit;
  io.Address  = WM8978_I2C_ADDR_DEFAULT;
  io.WriteReg = WM8978_IO_WriteReg;
  io.ReadReg  = WM8978_IO_ReadReg;
  io.GetTick  = WM8978_IO_GetTick;

  if (WM8978_RegisterBusIO(&wm8978_obj, &io) != WM8978_OK)
    return WM8978_ERROR;

  /* Guitar-specific init (power, mixer, volumes) */
  if (WM8978_Init_Guitar(&wm8978_obj) != WM8978_OK)
    return WM8978_ERROR;

  /* Override R6: CLKSEL=0(direct MCLK), MCLKDIV=1(÷1)
   *   MCLK = 11.29 MHz from PA3 I2S2_MCK
   *   SYSCLK = MCLK / 1 = 11.29 MHz */
  port_write_reg(WM8978_REG_CLOCKING_1, WM8978_MCLKDIV_1);

  /* Override R7: SR=48k family (covers 44.1kHz) */
  port_write_reg(WM8978_REG_CLOCKING_2, WM8978_SR_48K);

  /* Headphone volume: 57 = 0dB */
  port_write_reg(WM8978_REG_LEFT_HP_VOL, 57);
  port_write_reg(WM8978_REG_RIGHT_HP_VOL, 57 | WM8978_BIT_HPVU);

  return WM8978_OK;
}

int32_t WM8978_PORT_Test(void)
{
  uint32_t chip_id = 0;
  WM8978_IO_t io = {0};

  io.Init     = WM8978_IO_Init;
  io.DeInit   = WM8978_IO_DeInit;
  io.Address  = WM8978_I2C_ADDR_DEFAULT;
  io.WriteReg = WM8978_IO_WriteReg;
  io.ReadReg  = WM8978_IO_ReadReg;
  io.GetTick  = WM8978_IO_GetTick;

  if (WM8978_RegisterBusIO(&wm8978_obj, &io) != WM8978_OK)
    return -1;

  if (WM8978_Driver.ReadID(&wm8978_obj, &chip_id) != WM8978_OK)
    return -2;

  return 0;
}

WM8978_Object_t* WM8978_PORT_GetObj(void)
{
  return &wm8978_obj;
}

void WM8978_PORT_EnableADC(void)
{
  /* Re-trigger R2: ADCEN + BOOSTEN + L/R output:
   *   ADCENL=1, ADCENR=1, BOOSTENL=1, BOOSTENR=1,
   *   LOUT1EN=1, ROUT1EN=1 (same as Init_Guitar) */
  uint16_t val = WM8978_BIT_ADCENL | WM8978_BIT_ADCENR
               | WM8978_BIT_BOOSTENL | WM8978_BIT_BOOSTENR
               | WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN;
  port_write_reg(WM8978_REG_POWER_MANAGEMENT_2, val);
}
