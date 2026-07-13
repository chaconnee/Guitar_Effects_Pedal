/**
  ******************************************************************************
  * @file    wm8978_reg.c
  * @author  MCD Application Team
  * @brief   This file provides unitary register function to control the WM8978
  *          Audio Codec driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "wm8978_reg.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup wm8978
  * @brief     This file provides a set of functions needed to drive the
  *            WM8978 audio codec.
  * @{
  */

/************** Generic Function  *******************/

/*******************************************************************************
* Function Name : wm8978_write_reg
* Description   : Generic Writing function. It must be full-filled with either
*                 I2C or SPI writing functions.
*                 WM8978 9-bit register write protocol:
*                 Byte 1 = ((reg_addr[6:0] << 1) | data[8])
*                 Byte 2 = data[7:0]
* Input         : Register Address, data to be written, length of buffer
* Output        : None
* Return        : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_write_reg(wm8978_ctx_t *ctx, uint16_t reg, uint16_t *pdata, uint16_t length)
{
  uint8_t combined_byte;
  uint8_t data_low;
  (void)length;
  combined_byte = (uint8_t)(((reg & 0x7FU) << 1) | (((*pdata) >> 8) & 0x01U));
  data_low      = (uint8_t)((*pdata) & 0xFFU);
  return ctx->WriteReg(ctx->handle, (uint16_t)combined_byte, &data_low, 1U);
}

/*******************************************************************************
* Function Name : wm8978_read_reg
* Description   : Generic Reading function. It must be full-filled with either
*                 I2C or SPI reading functions.
*                 WM8978 9-bit register read protocol:
*                 Send combined_byte = (reg_addr[6:0] << 1), read 2 bytes back.
*                 B8 from bit0 of returned combined_byte, B7:0 from second byte.
* Input         : Register Address, length of buffer
* Output        : data Read
* Return        : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_read_reg(wm8978_ctx_t *ctx, uint16_t reg, uint16_t *pdata, uint16_t length)
{
  int32_t ret;
  uint8_t buf[2];
  uint8_t combined_byte;
  (void)length;
  combined_byte = (uint8_t)((reg & 0x7FU) << 1);
  ret = ctx->ReadReg(ctx->handle, (uint16_t)combined_byte, buf, 2U);
  if (ret == 0)
  {
    *pdata = (uint16_t)(((uint16_t)(combined_byte & 0x01U) << 8) | (uint16_t)buf[0]);
  }
  return ret;
}

/**************** Base Function  *******************/

/*******************************************************************************
* Function Name  : wm8978_register_set
* Description    : Write value to a WM8978 register
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_register_set(wm8978_ctx_t *ctx, uint16_t reg, uint16_t value)
{
  return wm8978_write_reg(ctx, reg, &value, 1U);
}

/************** Static Helper Function  *******************/

/*******************************************************************************
* Function Name  : wm8978_read_modify_write
* Description    : Read-Modify-Write helper for bit-field manipulation.
*                  Reads the register, clears the masked bits, sets the new
*                  value shifted into position, and writes back.
* Input          : reg - register address
*                  mask, pos - bit mask and position of the field
*                  value - new field value to write
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
static int32_t wm8978_read_modify_write(wm8978_ctx_t *ctx, uint16_t reg,
                                         uint16_t mask, uint16_t pos, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0U;
  ret = wm8978_read_reg(ctx, reg, &tmp, 1U);
  if (ret == 0)
  {
    tmp &= ~mask;
    tmp |= (value << pos) & mask;
    ret = wm8978_write_reg(ctx, reg, &tmp, 1U);
  }
  return ret;
}

/**************** Register Access Functions  *******************/

/*******************************************************************************
* Function Name  : wm8978_sw_reset_w
* Description    : Write Software Reset
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_sw_reset_w(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_write_reg(ctx, WM8978_SW_RESET, &value, 1U);
}

/*******************************************************************************
* Function Name  : wm8978_sw_reset_r
* Description    : Read Software Reset
* Input          : Pointer to uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_sw_reset_r(wm8978_ctx_t *ctx, uint16_t *value)
{
  return wm8978_read_reg(ctx, WM8978_SW_RESET, value, 1U);
}

/**************** Power Management 1 (0x01) *******************/

/*******************************************************************************
* Function Name  : wm8978_pwr1_vmid_sel
* Description    : VMID Divider Enable and Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr1_vmid_sel(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_1,
                                  WM8978_PWR1_VMIDSEL_MASK, WM8978_PWR1_VMIDSEL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr1_bias_en
* Description    : Bias Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr1_bias_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_1,
                                  WM8978_PWR1_BIASEN_MASK, WM8978_PWR1_BIASEN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr1_micb_en
* Description    : Microphone Bias Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr1_micb_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_1,
                                  WM8978_PWR1_MICBEN_MASK, WM8978_PWR1_MICBEN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr1_pll_en
* Description    : PLL Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr1_pll_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_1,
                                  WM8978_PWR1_PLLEN_MASK, WM8978_PWR1_PLLEN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr1_out3mix_en
* Description    : OUT3 Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr1_out3mix_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_1,
                                  WM8978_PWR1_OUT3MIXEN_MASK, WM8978_PWR1_OUT3MIXEN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr1_out4mix_en
* Description    : OUT4 Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr1_out4mix_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_1,
                                  WM8978_PWR1_OUT4MIXEN_MASK, WM8978_PWR1_OUT4MIXEN_POS, value);
}

/**************** Power Management 2 (0x02) *******************/

/*******************************************************************************
* Function Name  : wm8978_pwr2_adcl_en
* Description    : Left ADC Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_adcl_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_ADCENL_MASK, WM8978_PWR2_ADCENL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_adcr_en
* Description    : Right ADC Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_adcr_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_ADCENR_MASK, WM8978_PWR2_ADCENR_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_inppgal_en
* Description    : Left Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_inppgal_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_INPPGAENL_MASK, WM8978_PWR2_INPPGAENL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_inppgar_en
* Description    : Right Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_inppgar_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_INPPGAENR_MASK, WM8978_PWR2_INPPGAENR_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_boostl_en
* Description    : Left ADC Boost Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_boostl_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_BOOSTENL_MASK, WM8978_PWR2_BOOSTENL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_boostr_en
* Description    : Right ADC Boost Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_boostr_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_BOOSTENR_MASK, WM8978_PWR2_BOOSTENR_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_sleep
* Description    : Sleep Mode
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_sleep(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_SLEEP_MASK, WM8978_PWR2_SLEEP_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_lout1_en
* Description    : LOUT1 (Headphone Left) Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_lout1_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_LOUT1EN_MASK, WM8978_PWR2_LOUT1EN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr2_rout1_en
* Description    : ROUT1 (Headphone Right) Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr2_rout1_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_2,
                                  WM8978_PWR2_ROUT1EN_MASK, WM8978_PWR2_ROUT1EN_POS, value);
}

/**************** Power Management 3 (0x03) *******************/

/*******************************************************************************
* Function Name  : wm8978_pwr3_dacl_en
* Description    : Left DAC Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_dacl_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_DACENL_MASK, WM8978_PWR3_DACENL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr3_dacr_en
* Description    : Right DAC Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_dacr_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_DACENR_MASK, WM8978_PWR3_DACENR_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr3_lmix_en
* Description    : Left Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_lmix_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_LMIXEN_MASK, WM8978_PWR3_LMIXEN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr3_rmix_en
* Description    : Right Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_rmix_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_RMIXEN_MASK, WM8978_PWR3_RMIXEN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr3_lout2_en
* Description    : LOUT2 (Speaker Left) Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_lout2_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_LOUT2EN_MASK, WM8978_PWR3_LOUT2EN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr3_rout2_en
* Description    : ROUT2 (Speaker Right) Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_rout2_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_ROUT2EN_MASK, WM8978_PWR3_ROUT2EN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr3_out3_en
* Description    : OUT3 Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_out3_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_OUT3EN_MASK, WM8978_PWR3_OUT3EN_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_pwr3_out4_en
* Description    : OUT4 Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_pwr3_out4_en(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_PWR_MANAGEMENT_3,
                                  WM8978_PWR3_OUT4EN_MASK, WM8978_PWR3_OUT4EN_POS, value);
}

/**************** Audio Interface (0x04) *******************/

/*******************************************************************************
* Function Name  : wm8978_aif_fmt
* Description    : Audio Interface Format Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_aif_fmt(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_AUDIO_INTERFACE,
                                  WM8978_AIF_FMT_MASK, WM8978_AIF_FMT_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_aif_fmt_r
* Description    : Read Audio Interface Format
* Input          : Pointer to uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_aif_fmt_r(wm8978_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  uint16_t tmp = 0U;
  ret = wm8978_read_reg(ctx, WM8978_AUDIO_INTERFACE, &tmp, 1U);
  if (ret == 0)
  {
    *value = (tmp & WM8978_AIF_FMT_MASK) >> WM8978_AIF_FMT_POS;
  }
  return ret;
}

/*******************************************************************************
* Function Name  : wm8978_aif_wl
* Description    : Audio Interface Word Length Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_aif_wl(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_AUDIO_INTERFACE,
                                  WM8978_AIF_WL_MASK, WM8978_AIF_WL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_aif_wl_r
* Description    : Read Audio Interface Word Length
* Input          : Pointer to uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_aif_wl_r(wm8978_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  uint16_t tmp = 0U;
  ret = wm8978_read_reg(ctx, WM8978_AUDIO_INTERFACE, &tmp, 1U);
  if (ret == 0)
  {
    *value = (tmp & WM8978_AIF_WL_MASK) >> WM8978_AIF_WL_POS;
  }
  return ret;
}

/*******************************************************************************
* Function Name  : wm8978_aif_lrp
* Description    : Audio Interface LRC Polarity
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_aif_lrp(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_AUDIO_INTERFACE,
                                  WM8978_AIF_LRP_MASK, WM8978_AIF_LRP_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_aif_bcp
* Description    : Audio Interface BCLK Polarity
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_aif_bcp(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_AUDIO_INTERFACE,
                                  WM8978_AIF_BCP_MASK, WM8978_AIF_BCP_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_aif_mono
* Description    : Audio Interface Mono Mode
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_aif_mono(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_AUDIO_INTERFACE,
                                  WM8978_AIF_MONO_MASK, WM8978_AIF_MONO_POS, value);
}

/**************** Clock Gen Control (0x06) *******************/

/*******************************************************************************
* Function Name  : wm8978_clk_ms
* Description    : Clock Master/Slave Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_clk_ms(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_CLOCK_GEN_CTRL,
                                  WM8978_CLK_MS_MASK, WM8978_CLK_MS_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_clk_clksel
* Description    : Clock Source Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_clk_clksel(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_CLOCK_GEN_CTRL,
                                  WM8978_CLK_CLKSEL_MASK, WM8978_CLK_CLKSEL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_clk_mclkdiv
* Description    : MCLK Divider Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_clk_mclkdiv(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_CLOCK_GEN_CTRL,
                                  WM8978_CLK_MCLKDIV_MASK, WM8978_CLK_MCLKDIV_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_clk_bclkdiv
* Description    : BCLK Divider Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_clk_bclkdiv(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_CLOCK_GEN_CTRL,
                                  WM8978_CLK_BCLKDIV_MASK, WM8978_CLK_BCLKDIV_POS, value);
}

/**************** Additional Control (0x07) *******************/

/*******************************************************************************
* Function Name  : wm8978_add_sr
* Description    : Sample Rate Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_add_sr(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_ADDITIONAL_CTRL,
                                  WM8978_ADD_SR_MASK, WM8978_ADD_SR_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_add_sr_r
* Description    : Read Sample Rate Control
* Input          : Pointer to uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_add_sr_r(wm8978_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  uint16_t tmp = 0U;
  ret = wm8978_read_reg(ctx, WM8978_ADDITIONAL_CTRL, &tmp, 1U);
  if (ret == 0)
  {
    *value = (tmp & WM8978_ADD_SR_MASK) >> WM8978_ADD_SR_POS;
  }
  return ret;
}

/*******************************************************************************
* Function Name  : wm8978_add_slowclken
* Description    : Slow Clock Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_add_slowclken(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_ADDITIONAL_CTRL,
                                  WM8978_ADD_SLOWCLKEN_MASK, WM8978_ADD_SLOWCLKEN_POS, value);
}

/**************** DAC Control (0x0A) *******************/

/*******************************************************************************
* Function Name  : wm8978_dac_softmute
* Description    : DAC Soft Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_dac_softmute(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_DAC_CTRL,
                                  WM8978_DAC_SOFTMUTE_MASK, WM8978_DAC_SOFTMUTE_POS, value);
}

/**************** Left DAC Volume (0x0B) *******************/

/*******************************************************************************
* Function Name  : wm8978_ldac_voll
* Description    : Left DAC Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_ldac_voll(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_DAC_VOL,
                                  WM8978_LDAC_DACVOLL_MASK, WM8978_LDAC_DACVOLL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_ldac_vu
* Description    : Left DAC Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_ldac_vu(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_DAC_VOL,
                                  WM8978_LDAC_DACVU_MASK, WM8978_LDAC_DACVU_POS, value);
}

/**************** Right DAC Volume (0x0C) *******************/

/*******************************************************************************
* Function Name  : wm8978_rdac_volr
* Description    : Right DAC Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rdac_volr(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_RIGHT_DAC_VOL,
                                  WM8978_RDAC_DACVOLR_MASK, WM8978_RDAC_DACVOLR_POS, value);
}

/**************** Left Mixer Control (0x32) *******************/

/*******************************************************************************
* Function Name  : wm8978_lmix_dacl2lmix
* Description    : Left DAC to Left Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lmix_dacl2lmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_MIXER,
                                  WM8978_LMIX_DACL2LMIX_MASK, WM8978_LMIX_DACL2LMIX_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_lmix_bypl2lmix
* Description    : Left Bypass to Left Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lmix_bypl2lmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_MIXER,
                                  WM8978_LMIX_BYPL2LMIX_MASK, WM8978_LMIX_BYPL2LMIX_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_lmix_auxl2lmix
* Description    : Left Auxiliary to Left Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lmix_auxl2lmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_MIXER,
                                  WM8978_LMIX_AUXL2LMIX_MASK, WM8978_LMIX_AUXL2LMIX_POS, value);
}

/**************** Right Mixer Control (0x33) *******************/

/*******************************************************************************
* Function Name  : wm8978_rmix_dacr2rmix
* Description    : Right DAC to Right Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rmix_dacr2rmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_RIGHT_MIXER,
                                  WM8978_RMIX_DACR2RMIX_MASK, WM8978_RMIX_DACR2RMIX_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_rmix_bypr2rmix
* Description    : Right Bypass to Right Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rmix_bypr2rmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_RIGHT_MIXER,
                                  WM8978_RMIX_BYPR2RMIX_MASK, WM8978_RMIX_BYPR2RMIX_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_rmix_auxr2rmix
* Description    : Right Auxiliary to Right Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rmix_auxr2rmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_RIGHT_MIXER,
                                  WM8978_RMIX_AUXR2RMIX_MASK, WM8978_RMIX_AUXR2RMIX_POS, value);
}

/**************** Output Control (0x31) *******************/

/*******************************************************************************
* Function Name  : wm8978_outctrl_dacr2lmix
* Description    : Right DAC to Left Mixer Cross-Connect
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_outctrl_dacr2lmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_OUTPUT_CTRL,
                                  WM8978_OUTCTRL_DACR2LMIX_MASK, WM8978_OUTCTRL_DACR2LMIX_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_outctrl_dacl2rmix
* Description    : Left DAC to Right Mixer Cross-Connect
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_outctrl_dacl2rmix(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_OUTPUT_CTRL,
                                  WM8978_OUTCTRL_DACL2RMIX_MASK, WM8978_OUTCTRL_DACL2RMIX_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_outctrl_spkboost
* Description    : Speaker Boost Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_outctrl_spkboost(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_OUTPUT_CTRL,
                                  WM8978_OUTCTRL_SPKBOOST_MASK, WM8978_OUTCTRL_SPKBOOST_POS, value);
}

/**************** LOUT1 Volume Control (0x34) *******************/

/*******************************************************************************
* Function Name  : wm8978_lout1_vol
* Description    : LOUT1 (Headphone Left) Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lout1_vol(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LOUT1_VOL,
                                  WM8978_LOUT1_LOUT1VOL_MASK, WM8978_LOUT1_LOUT1VOL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_lout1_mute
* Description    : LOUT1 (Headphone Left) Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lout1_mute(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LOUT1_VOL,
                                  WM8978_LOUT1_LOUT1MUTE_MASK, WM8978_LOUT1_LOUT1MUTE_POS, value);
}

/**************** ROUT1 Volume Control (0x35) *******************/

/*******************************************************************************
* Function Name  : wm8978_rout1_vol
* Description    : ROUT1 (Headphone Right) Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rout1_vol(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_ROUT1_VOL,
                                  WM8978_ROUT1_ROUT1VOL_MASK, WM8978_ROUT1_ROUT1VOL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_rout1_mute
* Description    : ROUT1 (Headphone Right) Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rout1_mute(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_ROUT1_VOL,
                                  WM8978_ROUT1_ROUT1MUTE_MASK, WM8978_ROUT1_ROUT1MUTE_POS, value);
}

/**************** LOUT2 Volume Control (0x36) *******************/

/*******************************************************************************
* Function Name  : wm8978_lout2_vol
* Description    : LOUT2 (Speaker Left) Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lout2_vol(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LOUT2_VOL,
                                  WM8978_LOUT2_LOUT2VOL_MASK, WM8978_LOUT2_LOUT2VOL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_lout2_mute
* Description    : LOUT2 (Speaker Left) Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lout2_mute(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LOUT2_VOL,
                                  WM8978_LOUT2_LOUT2MUTE_MASK, WM8978_LOUT2_LOUT2MUTE_POS, value);
}

/**************** ROUT2 Volume Control (0x37) *******************/

/*******************************************************************************
* Function Name  : wm8978_rout2_vol
* Description    : ROUT2 (Speaker Right) Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rout2_vol(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_ROUT2_VOL,
                                  WM8978_ROUT2_ROUT2VOL_MASK, WM8978_ROUT2_ROUT2VOL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_rout2_mute
* Description    : ROUT2 (Speaker Right) Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rout2_mute(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_ROUT2_VOL,
                                  WM8978_ROUT2_ROUT2MUTE_MASK, WM8978_ROUT2_ROUT2MUTE_POS, value);
}

/**************** Left Input PGA Gain Control (0x2D) *******************/

/*******************************************************************************
* Function Name  : wm8978_lpga_voll
* Description    : Left Input PGA Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lpga_voll(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_INP_PGA_GAIN,
                                  WM8978_LPGA_INPPGAVOLL_MASK, WM8978_LPGA_INPPGAVOLL_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_lpga_mutel
* Description    : Left Input PGA Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lpga_mutel(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_INP_PGA_GAIN,
                                  WM8978_LPGA_INPPGAMUTEL_MASK, WM8978_LPGA_INPPGAMUTEL_POS, value);
}

/**************** Right Input PGA Gain Control (0x2E) *******************/

/*******************************************************************************
* Function Name  : wm8978_rpga_volr
* Description    : Right Input PGA Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rpga_volr(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_RIGHT_INP_PGA_GAIN,
                                  WM8978_RPGA_INPPGAVOLR_MASK, WM8978_RPGA_INPPGAVOLR_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_rpga_muter
* Description    : Right Input PGA Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rpga_muter(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_RIGHT_INP_PGA_GAIN,
                                  WM8978_RPGA_INPPGAMUTER_MASK, WM8978_RPGA_INPPGAMUTER_POS, value);
}

/**************** Left ADC Boost Control (0x2F) *******************/

/*******************************************************************************
* Function Name  : wm8978_lboost_pgaboostl
* Description    : Left PGA Boost Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_lboost_pgaboostl(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_LEFT_ADC_BOOST,
                                  WM8978_LBOOST_PGABOOSTL_MASK, WM8978_LBOOST_PGABOOSTL_POS, value);
}

/**************** Right ADC Boost Control (0x30) *******************/

/*******************************************************************************
* Function Name  : wm8978_rboost_pgaboostr
* Description    : Right PGA Boost Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_rboost_pgaboostr(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_RIGHT_ADC_BOOST,
                                  WM8978_RBOOST_PGABOOSTR_MASK, WM8978_RBOOST_PGABOOSTR_POS, value);
}

/**************** Input Control (0x2C) *******************/

/*******************************************************************************
* Function Name  : wm8978_inctrl_lip2inppga
* Description    : LIP Pin to Left Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_inctrl_lip2inppga(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_INPUT_CTRL,
                                  WM8978_INCTRL_LIP2INPPGA_MASK, WM8978_INCTRL_LIP2INPPGA_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_inctrl_lin2inppga
* Description    : LIN Pin to Left Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_inctrl_lin2inppga(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_INPUT_CTRL,
                                  WM8978_INCTRL_LIN2INPPGA_MASK, WM8978_INCTRL_LIN2INPPGA_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_inctrl_rip2inppga
* Description    : RIP Pin to Right Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_inctrl_rip2inppga(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_INPUT_CTRL,
                                  WM8978_INCTRL_RIP2INPPGA_MASK, WM8978_INCTRL_RIP2INPPGA_POS, value);
}

/*******************************************************************************
* Function Name  : wm8978_inctrl_rin2inppga
* Description    : RIN Pin to Right Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_inctrl_rin2inppga(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_INPUT_CTRL,
                                  WM8978_INCTRL_RIN2INPPGA_MASK, WM8978_INCTRL_RIN2INPPGA_POS, value);
}

/**************** Beep Control (0x2B) *******************/

/*******************************************************************************
* Function Name  : wm8978_beep_invrout2
* Description    : Beep Invert ROUT2 Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8978_ERROR, WM8978_OK]
*******************************************************************************/
int32_t wm8978_beep_invrout2(wm8978_ctx_t *ctx, uint16_t value)
{
  return wm8978_read_modify_write(ctx, WM8978_BEEP_CONTROL,
                                  WM8978_BEEP_INVROUT2_MASK, WM8978_BEEP_INVROUT2_POS, value);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
