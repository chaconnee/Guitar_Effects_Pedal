/**
  ******************************************************************************
  * @file    wm8978.c
  * @brief   WM8978 Audio Codec Driver Implementation
  *          Stereo CODEC with Speaker Driver
  *          Based on WM8978 datasheet Rev 4.5 and STM32 WM8994 driver style
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "wm8978.h"

/** @addtogroup WM8978
  * @{
  */

/** @defgroup WM8978_Private_Types Private Types
  * @{
  */
/* Audio codec driver structure initialization */
WM8978_Drv_t WM8978_Driver =
{
  WM8978_Init,
  WM8978_DeInit,
  WM8978_ReadID,
  WM8978_Play,
  WM8978_Pause,
  WM8978_Resume,
  WM8978_Stop,
  WM8978_SetFrequency,
  WM8978_GetFrequency,
  WM8978_SetVolume,
  WM8978_GetVolume,
  WM8978_SetMute,
  WM8978_SetOutputMode,
  WM8978_SetResolution,
  WM8978_GetResolution,
  WM8978_SetProtocol,
  WM8978_GetProtocol,
  WM8978_Reset
};

/**
  * @}
  */

/** @defgroup WM8978_Private_Defines Private Defines
  * @{
  */
/* Default register values for WM8978 */
static const uint16_t WM8978_REG_DEFAULTS[WM8978_REGISTER_COUNT] =
{
  0x0000, /* R0  - Software Reset */
  0x0000, /* R1  - Power Management 1 */
  0x0000, /* R2  - Power Management 2 */
  0x0000, /* R3  - Power Management 3 */
  0x0050, /* R4  - Audio Interface 1 (I2S, 16-bit) */
  0x0000, /* R5  - Audio Interface 2 */
  0x0140, /* R6  - Clocking 1 (MCLK/2, PLL) */
  0x0000, /* R7  - Clocking 2 */
  0x0000, /* R8  - GPIO Control */
  0x0000, /* R9  - Jack Detect Control 1 */
  0x0000, /* R10 - DAC Control */
  0x00FF, /* R11 - Left DAC Digital Volume */
  0x00FF, /* R12 - Right DAC Digital Volume */
  0x0000, /* R13 - Jack Detect Control 2 */
  0x0100, /* R14 - ADC Control */
  0x00FF, /* R15 - Left ADC Digital Volume */
  0x00FF, /* R16 - Right ADC Digital Volume */
  0x0000, /* R17 - EQ1 - Low Shelf */
  0x012C, /* R18 - EQ2 - Peaking */
  0x002C, /* R19 - EQ3 - Peaking */
  0x002C, /* R20 - EQ4 - Peaking */
  0x002C, /* R21 - EQ5 - High Shelf */
  0x0000, /* R22 - DAC Limitter 1 */
  0x0032, /* R23 - DAC Limitter 2 */
  0x0000, /* R24 - Notch Filter 1 */
  0x0000, /* R25 - Notch Filter 2 */
  0x0000, /* R26 - Notch Filter 3 */
  0x0000, /* R27 - Notch Filter 4 */
  0x0038, /* R28 - ALC Control 1 */
  0x000B, /* R29 - ALC Control 2 */
  0x0032, /* R30 - ALC Control 3 */
  0x0000, /* R31 - Noise Gate */
  0x0008, /* R32 - PLL N */
  0x000C, /* R33 - PLL K 1 */
  0x0093, /* R34 - PLL K 2 */
  0x00E9, /* R35 - PLL K 3 */
  0x0000, /* R36 - PLL Control */
  0x0000, /* R37 - BCLK/LRC Control */
  0x0000, /* R38 - Left Speaker Mixer */
  0x0003, /* R39 - Right Speaker Mixer */
  0x0010, /* R40 - Left Output Mixer */
  0x0010, /* R41 - Right Output Mixer */
  0x0100, /* R42 - Mono Output Mixer */
  0x0100, /* R43 - Beep Control */
  0x0002, /* R44 - Input Control */
  0x0001, /* R45 - Left Input PGA */
  0x0001, /* R46 - Right Input PGA */
  0x0039, /* R47 - Left ADC Boost */
  0x0039, /* R48 - Right ADC Boost */
  0x0039, /* R49 - Output Control */
  0x0039, /* R50 - Left Mixer Control */
  0x0001, /* R51 - Right Mixer Control */
  0x0001, /* R52 - Left Headphone Out Volume */
  0x0001, /* R53 - Right Headphone Out Volume */
  0x0001, /* R54 - Left Speaker Out Volume */
  0x0001  /* R55 - Right Speaker Out Volume */
};

/**
  * @}
  */

/** @defgroup WM8978_Function_Prototypes Function Prototypes
  * @{
  */
static int32_t WM8978_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t WM8978_Delay(WM8978_Object_t *pObj, uint32_t Delay);
static int32_t WM8978_WriteRegister(WM8978_Object_t *pObj, uint8_t Reg, uint16_t Value);
static int32_t WM8978_ReadRegister(WM8978_Object_t *pObj, uint8_t Reg, uint16_t *Value);
static int32_t WM8978_UpdateRegister(WM8978_Object_t *pObj, uint8_t Reg, uint16_t Mask, uint16_t Value);

/**
  * @}
  */

/** @defgroup WM8978_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief Initializes the audio codec and the control interface.
  * @param pObj pointer to component object
  * @param pInit pointer to component init structure
  * @retval WM8978_OK if correct communication, else WM8978_ERROR
  */
int32_t WM8978_Init(WM8978_Object_t *pObj, WM8978_Init_t *pInit)
{
  int32_t ret = WM8978_OK;

  /* Reset the codec */
  ret = WM8978_Reset(pObj);
  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  /* Add small delay after reset */
  (void)WM8978_Delay(pObj, 10);

  /* Power Management 1: Enable Bias, VMID, MIC Bias */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_1,
                               WM8978_BIT_BIASEN | WM8978_BIT_BUFIOEN | WM8978_VMIDSEL_5K);

  /* Add delay for VMID startup */
  (void)WM8978_Delay(pObj, 50);

  /* Configure input path */
  if (pInit->InputDevice != WM8978_IN_NONE)
  {
    /* Enable MIC Bias if needed */
    if ((pInit->InputDevice == WM8978_IN_MIC1) || (pInit->InputDevice == WM8978_IN_MIC2))
    {
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_1,
                                    WM8978_BIT_MICBEN, WM8978_BIT_MICBEN);
    }

    /* Power Management 2: Enable ADC and Input PGA */
    ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                 WM8978_BIT_ADCENL | WM8978_BIT_ADCENR);

    /* Input Control: Configure input routing */
    switch (pInit->InputDevice)
    {
    case WM8978_IN_MIC1:
      ret += WM8978_WriteRegister(pObj, WM8978_REG_INPUT_CONTROL,
                                   WM8978_BIT_LIP1INPGA | WM8978_BIT_RIP1INPGA);
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                    WM8978_BIT_INPPGAENL | WM8978_BIT_INPPGAENR | WM8978_BIT_BOOSTENL | WM8978_BIT_BOOSTENR,
                                    WM8978_BIT_INPPGAENL | WM8978_BIT_INPPGAENR | WM8978_BIT_BOOSTENL | WM8978_BIT_BOOSTENR);
      break;

    case WM8978_IN_MIC2:
      ret += WM8978_WriteRegister(pObj, WM8978_REG_INPUT_CONTROL,
                                   WM8978_BIT_LIN2INPPGA | WM8978_BIT_RIN2INPPGA);
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                    WM8978_BIT_INPPGAENL | WM8978_BIT_INPPGAENR | WM8978_BIT_BOOSTENL | WM8978_BIT_BOOSTENR,
                                    WM8978_BIT_INPPGAENL | WM8978_BIT_INPPGAENR | WM8978_BIT_BOOSTENL | WM8978_BIT_BOOSTENR);
      break;

    case WM8978_IN_LINE1:
      ret += WM8978_WriteRegister(pObj, WM8978_REG_INPUT_CONTROL,
                                   WM8978_BIT_LIP1INPGA | WM8978_BIT_RIP1INPGA);
      break;

    case WM8978_IN_LINE2:
      ret += WM8978_WriteRegister(pObj, WM8978_REG_INPUT_CONTROL,
                                   WM8978_BIT_LIN2INPPGA | WM8978_BIT_RIN2INPPGA);
      break;

    case WM8978_IN_AUX:
      /* AUX input via mixer */
      break;

    default:
      break;
    }

    /* Set default ADC volume */
    ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_ADC_VOL, 0x00C3);
    ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_ADC_VOL, 0x00C3);
  }

  /* Configure output path */
  if (pInit->OutputDevice != WM8978_OUT_NONE)
  {
    /* Power Management 3: Enable DAC and Output Mixers */
    ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
                                 WM8978_BIT_DACENL | WM8978_BIT_DACENR | WM8978_BIT_LMIXEN | WM8978_BIT_RMIXEN);

    switch (pInit->OutputDevice)
    {
    case WM8978_OUT_HEADPHONE:
      /* Enable LOUT1/ROUT1 */
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                    WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN,
                                    WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN);
      /* Mixer: DAC to output */
      ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_MIXER_CONTROL, WM8978_BIT_LD2LMIX);
      ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_MIXER_CONTROL, WM8978_BIT_LD2LMIX);
      break;

    case WM8978_OUT_SPEAKER:
      /* Enable LOUT2/ROUT2 */
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
                                    WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN,
                                    WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN);
      /* Enable speaker boost */
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_OUTPUT_CONTROL,
                                    WM8978_BIT_SPKBOOST, WM8978_BIT_SPKBOOST);
      /* Mixer: DAC to output */
      ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_MIXER_CONTROL, WM8978_BIT_LD2LMIX);
      ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_MIXER_CONTROL, WM8978_BIT_LD2LMIX);
      break;

    case WM8978_OUT_BOTH:
      /* Enable both HP and Speaker outputs */
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                    WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN,
                                    WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN);
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
                                    WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN,
                                    WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN);
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_OUTPUT_CONTROL,
                                    WM8978_BIT_SPKBOOST, WM8978_BIT_SPKBOOST);
      /* Mixer: DAC to output */
      ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_MIXER_CONTROL, WM8978_BIT_LD2LMIX);
      ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_MIXER_CONTROL, WM8978_BIT_LD2LMIX);
      break;

    case WM8978_OUT_LINE:
      /* Enable OUT3/OUT4 */
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_1,
                                    WM8978_BIT_OUT3MIXEN | WM8978_BIT_OUT4MIXEN,
                                    WM8978_BIT_OUT3MIXEN | WM8978_BIT_OUT4MIXEN);
      ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
                                    WM8978_BIT_OUT3EN | WM8978_BIT_OUT4EN,
                                    WM8978_BIT_OUT3EN | WM8978_BIT_OUT4EN);
      break;

    default:
      break;
    }

    /* Set default DAC volume (max) */
    ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_DAC_VOL, 0x00FF);
    ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_DAC_VOL, 0x00FF);

    /* Unmute DAC */
    ret += WM8978_WriteRegister(pObj, WM8978_REG_DAC_CONTROL, 0x0000);

    /* Set headphone volume */
    ret += WM8978_SetVolume(pObj, VOLUME_OUTPUT, (uint8_t)pInit->Volume);
  }

  /* Configure I2S interface */
  ret += WM8978_SetResolution(pObj, pInit->Resolution);
  ret += WM8978_SetProtocol(pObj, WM8978_PROTOCOL_I2S);

  /* Configure clocking: MCLK, auto clock select */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_CLOCKING_1,
                               WM8978_BIT_CLKSEL | WM8978_MCLKDIV_2);

  /* Configure sample rate */
  ret += WM8978_SetFrequency(pObj, pInit->Frequency);

  /* Enable thermal shutdown protection */
  ret += WM8978_UpdateRegister(pObj, WM8978_REG_OUTPUT_CONTROL,
                                WM8978_BIT_TSDEN, WM8978_BIT_TSDEN);

  /* Configure OUT3/OUT4 if needed */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_OUT3_MIXER_CONTROL, WM8978_BIT_LDAC2OUT3);
  ret += WM8978_WriteRegister(pObj, WM8978_REG_OUT4_MIXER_CONTROL, WM8978_BIT_RDAC2OUT4);

  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  pObj->IsInitialized = 1;

  return WM8978_OK;
}

/**
  * @brief Deinitializes the audio codec.
  * @param pObj pointer to component object
  * @retval Component status
  */
int32_t WM8978_DeInit(WM8978_Object_t *pObj)
{
  return WM8978_Stop(pObj, WM8978_PDWN_HW);
}

/**
  * @brief Get the WM8978 ID (reads register 0 after reset).
  * @param pObj pointer to component object
  * @param Id component ID (will be 0 for WM8978 as it has no ID register)
  * @retval Component status
  */
int32_t WM8978_ReadID(WM8978_Object_t *pObj, uint32_t *Id)
{
  int32_t ret;
  uint16_t wm8978_id;

  /* Initialize the Control interface of the Audio Codec */
  pObj->IO.Init();

  /* WM8978 doesn't have an ID register, read R0 (should return 0 after reset) */
  ret = WM8978_ReadRegister(pObj, WM8978_REG_SOFTWARE_RESET, &wm8978_id);

  *Id = (uint32_t)wm8978_id;

  return ret;
}

/**
  * @brief Start the audio Codec play feature.
  * @param pObj pointer to component object
  * @retval Component status
  */
int32_t WM8978_Play(WM8978_Object_t *pObj)
{
  /* Unmute the output */
  return WM8978_SetMute(pObj, WM8978_MUTE_OFF);
}

/**
  * @brief Pauses playing on the audio codec.
  * @param pObj pointer to component object
  * @retval Component status
  */
int32_t WM8978_Pause(WM8978_Object_t *pObj)
{
  int32_t ret;

  /* Mute the output first */
  ret = WM8978_SetMute(pObj, WM8978_MUTE_ON);

  return ret;
}

/**
  * @brief Resumes playing on the audio codec.
  * @param pObj pointer to component object
  * @retval Component status
  */
int32_t WM8978_Resume(WM8978_Object_t *pObj)
{
  /* Unmute the output */
  return WM8978_SetMute(pObj, WM8978_MUTE_OFF);
}

/**
  * @brief Stops audio Codec playing. It powers down the codec.
  * @param pObj pointer to component object
  * @param CodecPdwnMode selects the power down mode.
  *          - WM8978_PDWN_SW: only mutes the audio codec.
  *          - WM8978_PDWN_HW: physically power down the codec.
  * @retval Component status
  */
int32_t WM8978_Stop(WM8978_Object_t *pObj, uint32_t CodecPdwnMode)
{
  int32_t ret;

  /* Mute the output first */
  ret = WM8978_SetMute(pObj, WM8978_MUTE_ON);

  if (CodecPdwnMode == WM8978_PDWN_HW)
  {
    /* Power down all sections */
    ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_1, 0x0000);
    ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2, 0x0000);
    ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3, 0x0000);

    /* Reset the codec */
    ret += WM8978_Reset(pObj);
  }

  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  return WM8978_OK;
}

/**
  * @brief Set higher or lower the codec volume level.
  * @param pObj pointer to component object
  * @param InputOutput Input or Output volume
  * @param Volume a byte value from 0 to 63 for output and from 0 to 255 for input
  * @retval Component status
  */
int32_t WM8978_SetVolume(WM8978_Object_t *pObj, uint32_t InputOutput, uint8_t Volume)
{
  int32_t ret = WM8978_OK;
  uint16_t vol;

  if (InputOutput == VOLUME_OUTPUT)
  {
    /* Output volume: 0-63 */
    vol = (uint16_t)(Volume & 0x3F);

    if (Volume == 0U)
    {
      /* Mute */
      ret = WM8978_SetMute(pObj, WM8978_MUTE_ON);
    }
    else
    {
      /* Unmute */
      ret = WM8978_SetMute(pObj, WM8978_MUTE_OFF);

      /* Set headphone volume with update */
      ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_HP_VOL, vol);
      ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_HP_VOL, vol | WM8978_BIT_HPVU);

      /* Set speaker volume with update */
      ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_SPK_VOL, vol);
      ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_SPK_VOL, vol | WM8978_BIT_SPKVU);
    }
  }
  else /* VOLUME_INPUT */
  {
    /* Input volume: 0-255 */
    vol = (uint16_t)(Volume);

    /* Set ADC digital volume */
    ret = WM8978_WriteRegister(pObj, WM8978_REG_LEFT_ADC_VOL, vol);
    ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_ADC_VOL, vol);
  }

  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  return WM8978_OK;
}

/**
  * @brief Get the codec volume level.
  * @param pObj pointer to component object
  * @param InputOutput Input or Output volume
  * @param Volume pointer to volume value
  * @retval Component status
  */
int32_t WM8978_GetVolume(WM8978_Object_t *pObj, uint32_t InputOutput, uint8_t *Volume)
{
  int32_t ret;
  uint16_t vol;

  if (InputOutput == VOLUME_OUTPUT)
  {
    ret = WM8978_ReadRegister(pObj, WM8978_REG_LEFT_HP_VOL, &vol);
    if (ret == WM8978_OK)
    {
      *Volume = (uint8_t)(vol & 0x3F);
    }
  }
  else
  {
    ret = WM8978_ReadRegister(pObj, WM8978_REG_LEFT_ADC_VOL, &vol);
    if (ret == WM8978_OK)
    {
      *Volume = (uint8_t)(vol & 0xFF);
    }
  }

  return ret;
}

/**
  * @brief Enables or disables the mute feature on the audio codec.
  * @param pObj pointer to component object
  * @param Cmd WM8978_MUTE_ON to enable the mute or WM8978_MUTE_OFF to disable
  * @retval Component status
  */
int32_t WM8978_SetMute(WM8978_Object_t *pObj, uint32_t Cmd)
{
  int32_t ret;
  uint16_t dac_ctrl;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_DAC_CONTROL, &dac_ctrl);
  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  if (Cmd == WM8978_MUTE_ON)
  {
    /* Enable soft mute */
    dac_ctrl |= WM8978_BIT_AMUTE;
  }
  else
  {
    /* Disable soft mute */
    dac_ctrl &= ~WM8978_BIT_AMUTE;
  }

  ret = WM8978_WriteRegister(pObj, WM8978_REG_DAC_CONTROL, dac_ctrl);

  return ret;
}

/**
  * @brief Switch dynamically the output target.
  * @param pObj pointer to component object
  * @param Output specifies the audio output target
  * @retval Component status
  */
int32_t WM8978_SetOutputMode(WM8978_Object_t *pObj, uint32_t Output)
{
  int32_t ret = WM8978_OK;

  /* Disable all outputs first */
  ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN, 0);
  ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
                                WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN, 0);

  switch (Output)
  {
  case WM8978_OUT_HEADPHONE:
    ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                  WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN,
                                  WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN);
    break;

  case WM8978_OUT_SPEAKER:
    ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
                                  WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN,
                                  WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN);
    ret += WM8978_UpdateRegister(pObj, WM8978_REG_OUTPUT_CONTROL,
                                  WM8978_BIT_SPKBOOST, WM8978_BIT_SPKBOOST);
    break;

  case WM8978_OUT_BOTH:
    ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
                                  WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN,
                                  WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN);
    ret += WM8978_UpdateRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
                                  WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN,
                                  WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN);
    ret += WM8978_UpdateRegister(pObj, WM8978_REG_OUTPUT_CONTROL,
                                  WM8978_BIT_SPKBOOST, WM8978_BIT_SPKBOOST);
    break;

  default:
    break;
  }

  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  return WM8978_OK;
}

/**
  * @brief Set Audio resolution.
  * @param pObj pointer to component object
  * @param Resolution Audio resolution (WM8978_RESOLUTION_xx)
  * @retval Component status
  */
int32_t WM8978_SetResolution(WM8978_Object_t *pObj, uint32_t Resolution)
{
  int32_t ret;
  uint16_t aif1;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_AUDIO_INTERFACE_1, &aif1);
  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  /* Clear word length bits and set new value */
  aif1 &= ~WM8978_MASK_WL;
  aif1 |= (uint16_t)((Resolution & 0x03) << 5);

  ret = WM8978_WriteRegister(pObj, WM8978_REG_AUDIO_INTERFACE_1, aif1);

  return ret;
}

/**
  * @brief Get Audio resolution.
  * @param pObj pointer to component object
  * @param Resolution pointer to resolution value
  * @retval Component status
  */
int32_t WM8978_GetResolution(WM8978_Object_t *pObj, uint32_t *Resolution)
{
  int32_t ret;
  uint16_t aif1;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_AUDIO_INTERFACE_1, &aif1);
  if (ret == WM8978_OK)
  {
    *Resolution = (uint32_t)((aif1 & WM8978_MASK_WL) >> 5);
  }

  return ret;
}

/**
  * @brief Set Audio Protocol.
  * @param pObj pointer to component object
  * @param Protocol Audio Protocol (WM8978_PROTOCOL_xx)
  * @retval Component status
  */
int32_t WM8978_SetProtocol(WM8978_Object_t *pObj, uint32_t Protocol)
{
  int32_t ret;
  uint16_t aif1;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_AUDIO_INTERFACE_1, &aif1);
  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  /* Clear format bits and set new value */
  aif1 &= ~WM8978_MASK_FMT;
  aif1 |= (uint16_t)((Protocol & 0x03) << 3);

  ret = WM8978_WriteRegister(pObj, WM8978_REG_AUDIO_INTERFACE_1, aif1);

  return ret;
}

/**
  * @brief Get Audio Protocol.
  * @param pObj pointer to component object
  * @param Protocol pointer to protocol value
  * @retval Component status
  */
int32_t WM8978_GetProtocol(WM8978_Object_t *pObj, uint32_t *Protocol)
{
  int32_t ret;
  uint16_t aif1;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_AUDIO_INTERFACE_1, &aif1);
  if (ret == WM8978_OK)
  {
    *Protocol = (uint32_t)((aif1 & WM8978_MASK_FMT) >> 3);
  }

  return ret;
}

/**
  * @brief Sets new frequency.
  * @param pObj pointer to component object
  * @param AudioFreq Audio frequency
  * @retval Component status
  */
int32_t WM8978_SetFrequency(WM8978_Object_t *pObj, uint32_t AudioFreq)
{
  int32_t ret;
  uint16_t clocking2;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_CLOCKING_2, &clocking2);
  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  /* Clear sample rate bits */
  clocking2 &= ~WM8978_MASK_SR;

  switch (AudioFreq)
  {
  case WM8978_FREQUENCY_48K:
    clocking2 |= WM8978_SR_48K;
    break;

  case WM8978_FREQUENCY_32K:
    clocking2 |= WM8978_SR_32K;
    break;

  case WM8978_FREQUENCY_24K:
    clocking2 |= WM8978_SR_24K;
    break;

  case WM8978_FREQUENCY_16K:
    clocking2 |= WM8978_SR_16K;
    break;

  case WM8978_FREQUENCY_12K:
    clocking2 |= WM8978_SR_12K;
    break;

  case WM8978_FREQUENCY_8K:
    clocking2 |= WM8978_SR_8K;
    break;

  case WM8978_FREQUENCY_44K:
  case WM8978_FREQUENCY_22K:
  case WM8978_FREQUENCY_11K:
    /* For 44.1kHz family, use PLL to achieve correct rate */
    clocking2 |= WM8978_SR_48K; /* Base on 48k, PLL handles the rest */
    break;

  default:
    clocking2 |= WM8978_SR_48K;
    break;
  }

  ret = WM8978_WriteRegister(pObj, WM8978_REG_CLOCKING_2, clocking2);

  return ret;
}

/**
  * @brief Get frequency.
  * @param pObj pointer to component object
  * @param AudioFreq pointer to frequency value
  * @retval Component status
  */
int32_t WM8978_GetFrequency(WM8978_Object_t *pObj, uint32_t *AudioFreq)
{
  int32_t ret;
  uint16_t clocking2;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_CLOCKING_2, &clocking2);
  if (ret == WM8978_OK)
  {
    switch (clocking2 & WM8978_MASK_SR)
    {
    case WM8978_SR_48K:
      *AudioFreq = WM8978_FREQUENCY_48K;
      break;
    case WM8978_SR_32K:
      *AudioFreq = WM8978_FREQUENCY_32K;
      break;
    case WM8978_SR_24K:
      *AudioFreq = WM8978_FREQUENCY_24K;
      break;
    case WM8978_SR_16K:
      *AudioFreq = WM8978_FREQUENCY_16K;
      break;
    case WM8978_SR_12K:
      *AudioFreq = WM8978_FREQUENCY_12K;
      break;
    case WM8978_SR_8K:
      *AudioFreq = WM8978_FREQUENCY_8K;
      break;
    default:
      *AudioFreq = WM8978_FREQUENCY_48K;
      break;
    }
  }

  return ret;
}

/**
  * @brief Resets WM8978 registers.
  * @param pObj pointer to component object
  * @retval Component status
  */
int32_t WM8978_Reset(WM8978_Object_t *pObj)
{
  int32_t ret;
  uint16_t tmp = 0x0000;

  /* Reset Codec by writing 0x0000 to register 0 */
  ret = WM8978_WriteRegister(pObj, WM8978_REG_SOFTWARE_RESET, tmp);

  /* Restore default register values to cache */
  for (uint8_t i = 0; i < WM8978_REGISTER_COUNT; i++)
  {
    pObj->RegCache[i] = WM8978_REG_DEFAULTS[i];
  }

  return ret;
}

/**
  * @brief Set MIC gain (PGA gain).
  * @param pObj pointer to component object
  * @param Gain 0~63, -12dB to +35.25dB, 0.75dB/step
  * @retval Component status
  */
int32_t WM8978_SetMICGain(WM8978_Object_t *pObj, uint8_t Gain)
{
  int32_t ret;

  Gain &= 0x3F;

  ret = WM8978_WriteRegister(pObj, WM8978_REG_LEFT_INPUT_PGA, (uint16_t)Gain);
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_INPUT_PGA, (uint16_t)Gain | 0x100);

  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  return WM8978_OK;
}

/**
  * @brief Set LINE IN gain.
  * @param pObj pointer to component object
  * @param Gain 0~7, 0=mute, 1~7 = -12dB to +6dB, 3dB/step
  * @retval Component status
  */
int32_t WM8978_SetLINEINGain(WM8978_Object_t *pObj, uint8_t Gain)
{
  int32_t ret;
  uint16_t regval;
  uint16_t gain_val = (uint16_t)(Gain & 0x07);

  /* Left ADC Boost */
  ret = WM8978_ReadRegister(pObj, WM8978_REG_LEFT_ADC_BOOST, &regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  regval &= ~WM8978_MASK_L2_2BOOSTVOL;
  regval |= (gain_val << 4);

  ret = WM8978_WriteRegister(pObj, WM8978_REG_LEFT_ADC_BOOST, regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  /* Right ADC Boost */
  ret = WM8978_ReadRegister(pObj, WM8978_REG_RIGHT_ADC_BOOST, &regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  regval &= ~WM8978_MASK_R2_2BOOSTVOL;
  regval |= (gain_val << 4);

  ret = WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_ADC_BOOST, regval);

  return ret;
}

/**
  * @brief Set AUX gain.
  * @param pObj pointer to component object
  * @param Gain 0~7, 0=mute, 1~7 = -12dB to +6dB, 3dB/step
  * @retval Component status
  */
int32_t WM8978_SetAUXGain(WM8978_Object_t *pObj, uint8_t Gain)
{
  int32_t ret;
  uint16_t regval;
  uint16_t gain_val = (uint16_t)(Gain & 0x07);

  /* Left ADC Boost */
  ret = WM8978_ReadRegister(pObj, WM8978_REG_LEFT_ADC_BOOST, &regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  regval &= ~WM8978_MASK_AUXL2BOOSTVOL;
  regval |= gain_val;

  ret = WM8978_WriteRegister(pObj, WM8978_REG_LEFT_ADC_BOOST, regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  /* Right ADC Boost */
  ret = WM8978_ReadRegister(pObj, WM8978_REG_RIGHT_ADC_BOOST, &regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  regval &= ~WM8978_MASK_AUXR2BOOSTVOL;
  regval |= gain_val;

  ret = WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_ADC_BOOST, regval);

  return ret;
}

/**
  * @brief Set EQ band.
  * @param pObj pointer to component object
  * @param EQBand 1-5
  * @param Freq frequency selection (0-3)
  * @param Gain gain value (0-24, representing -12dB to +12dB)
  * @retval Component status
  */
int32_t WM8978_SetEQ(WM8978_Object_t *pObj, uint8_t EQBand, uint8_t Freq, uint8_t Gain)
{
  int32_t ret = WM8978_OK;
  uint16_t regval;
  uint8_t reg;

  if (EQBand < 1 || EQBand > 5) return WM8978_ERROR;
  if (Gain > 24) Gain = 24;

  /* Convert gain: 0=-12dB, 12=0dB, 24=+12dB */
  Gain = 24 - Gain;

  reg = WM8978_REG_EQ1_LOW_SHELF + (EQBand - 1);

  if (EQBand == 1)
  {
    /* EQ1 has the 3D/EQ select bit */
    ret = WM8978_ReadRegister(pObj, reg, &regval);
    if (ret != WM8978_OK) return WM8978_ERROR;

    regval &= 0x100; /* Preserve 3D/EQ bit */
    regval |= ((uint16_t)(Freq & 0x03) << 5);
    regval |= (uint16_t)(Gain & 0x1F);
  }
  else
  {
    regval = ((uint16_t)(Freq & 0x03) << 5) | (uint16_t)(Gain & 0x1F);
  }

  ret = WM8978_WriteRegister(pObj, reg, regval);

  return ret;
}

/**
  * @brief Set 3D effect depth.
  * @param pObj pointer to component object
  * @param Depth 0~15 (0=off, 15=max)
  * @retval Component status
  */
int32_t WM8978_Set3D(WM8978_Object_t *pObj, uint8_t Depth)
{
  int32_t ret;

  Depth &= 0x0F;

  ret = WM8978_WriteRegister(pObj, WM8978_REG_EQ1_LOW_SHELF, (uint16_t)Depth);

  return ret;
}

/**
  * @brief Set EQ/3D direction.
  * @param pObj pointer to component object
  * @param Dir 0=ADC path, 1=DAC path (default)
  * @retval Component status
  */
int32_t WM8978_SetEQ3DDirection(WM8978_Object_t *pObj, uint8_t Dir)
{
  int32_t ret;
  uint16_t regval;

  ret = WM8978_ReadRegister(pObj, WM8978_REG_EQ2_PEAKING, &regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  if (Dir)
  {
    regval |= (1 << 8);
  }
  else
  {
    regval &= ~(1 << 8);
  }

  ret = WM8978_WriteRegister(pObj, WM8978_REG_EQ2_PEAKING, regval);

  return ret;
}

/**
  * @brief Register Bus IO.
  * @param pObj pointer to component object
  * @param pIO pointer to IO structure
  * @retval Component status
  */
int32_t WM8978_RegisterBusIO(WM8978_Object_t *pObj, WM8978_IO_t *pIO)
{
  if (pObj == NULL)
  {
    return WM8978_ERROR;
  }

  pObj->IO.Init      = pIO->Init;
  pObj->IO.DeInit    = pIO->DeInit;
  pObj->IO.Address   = pIO->Address;
  pObj->IO.WriteReg  = pIO->WriteReg;
  pObj->IO.ReadReg   = pIO->ReadReg;
  pObj->IO.GetTick   = pIO->GetTick;

  if (pObj->IO.Init != NULL)
  {
    return pObj->IO.Init();
  }

  return WM8978_OK;
}

/**
  * @}
  */

/** @defgroup WM8978_Private_Functions Private Functions
  * @{
  */

/**
  * @brief Provides accurate delay (in milliseconds).
  * @param pObj pointer to component object
  * @param Delay specifies the delay time length, in milliseconds
  * @retval Component status
  */
static int32_t WM8978_Delay(WM8978_Object_t *pObj, uint32_t Delay)
{
  uint32_t tickstart;

  tickstart = pObj->IO.GetTick();
  while ((pObj->IO.GetTick() - tickstart) < Delay)
  {
  }

  return WM8978_OK;
}

/**
  * @brief Write register wrapper.
  * @param handle component object handle
  * @param Reg target register address
  * @param Data data to write
  * @param Length data length
  * @retval error status
  */
static int32_t WM8978_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length)
{
  WM8978_Object_t *pObj = (WM8978_Object_t *)handle;
  return pObj->IO.WriteReg(pObj->IO.Address, Reg, Data, Length);
}

/**
  * @brief Read register wrapper.
  * @param handle component object handle
  * @param Reg target register address
  * @param Data data buffer
  * @param Length data length
  * @retval error status
  */
/* Note: WM8978 registers are write-only, this wrapper is kept for potential future use */
/*
static int32_t WM8978_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length)
{
  WM8978_Object_t *pObj = (WM8978_Object_t *)handle;
  return pObj->IO.ReadReg(pObj->IO.Address, Reg, Data, Length);
}
*/

/**
  * @brief Write a 16-bit value to a WM8978 register.
  * @param pObj pointer to component object
  * @param Reg register address (7-bit)
  * @param Value 9-bit register value
  * @retval Component status
  */
static int32_t WM8978_WriteRegister(WM8978_Object_t *pObj, uint8_t Reg, uint16_t Value)
{
  int32_t ret;
  uint8_t data[2];

  /* WM8978 I2C format: [7-bit addr][W] [Reg[6:0], Value[8]] [Value[7:0]] */
  data[0] = (uint8_t)((Reg << 1) | ((Value >> 8) & 0x01));
  data[1] = (uint8_t)(Value & 0xFF);

  ret = WM8978_WriteRegWrap(pObj, (uint16_t)Reg, data, 2);

  if (ret == WM8978_OK)
  {
    /* Update register cache */
    if (Reg < WM8978_REGISTER_COUNT)
    {
      pObj->RegCache[Reg] = Value;
    }
  }

  return ret;
}

/**
  * @brief Read a 16-bit value from a WM8978 register.
  * @param pObj pointer to component object
  * @param Reg register address (7-bit)
  * @param Value pointer to 9-bit register value
  * @retval Component status
  */
static int32_t WM8978_ReadRegister(WM8978_Object_t *pObj, uint8_t Reg, uint16_t *Value)
{
  /* WM8978 registers are write-only, use cached values */
  if (Reg < WM8978_REGISTER_COUNT)
  {
    *Value = pObj->RegCache[Reg];
    return WM8978_OK;
  }

  return WM8978_ERROR;
}

/**
  * @brief Update specific bits in a register.
  * @param pObj pointer to component object
  * @param Reg register address
  * @param Mask bits to update
  * @param Value new value for masked bits
  * @retval Component status
  */
static int32_t WM8978_UpdateRegister(WM8978_Object_t *pObj, uint8_t Reg, uint16_t Mask, uint16_t Value)
{
  int32_t ret;
  uint16_t regval;

  ret = WM8978_ReadRegister(pObj, Reg, &regval);
  if (ret != WM8978_OK) return WM8978_ERROR;

  regval &= ~Mask;
  regval |= (Value & Mask);

  ret = WM8978_WriteRegister(pObj, Reg, regval);

  return ret;
}

/**
  * @brief Guitar-specific initialization.
  *        Signal path: Guitar → L2/R2 → BOOST → ADC → I2S → STM32
  *                     STM32 → I2S → DAC → LOUT1/ROUT1 (HP) + OUT3/OUT4 (TRS)
  * @param pObj pointer to component object (must have IO registered via WM8978_RegisterBusIO)
  * @retval Component status
  */
int32_t WM8978_Init_Guitar(WM8978_Object_t *pObj)
{
  int32_t ret = WM8978_OK;

  /* Reset the codec */
  ret = WM8978_Reset(pObj);
  if (ret != WM8978_OK) return WM8978_ERROR;

  (void)WM8978_Delay(pObj, 10);

  /* ---- R1: Power Management 1 ----
   * BIASEN=1, BUFIOEN=1, VMIDSEL=5kΩ
   * OUT3MIXEN=1, OUT4MIXEN=1
   * MICBEN=0 (no mic bias for guitar)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_1,
    WM8978_BIT_BIASEN | WM8978_BIT_BUFIOEN | WM8978_VMIDSEL_5K |
    WM8978_BIT_OUT3MIXEN | WM8978_BIT_OUT4MIXEN);

  (void)WM8978_Delay(pObj, 50);  /* Wait for VMID to stabilize */

  /* ---- R2: Power Management 2 ----
   * ADCENL=1, ADCENR=1 (enable ADC)
   * BOOSTENL=1, BOOSTENR=1 (enable input boost stage)
   * INPPGAENL=0, INPPGAENR=0 (bypass PGA - guitar doesn't need it)
   * LOUT1EN=1, ROUT1EN=1 (enable headphone output)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_2,
    WM8978_BIT_ADCENL | WM8978_BIT_ADCENR |
    WM8978_BIT_BOOSTENL | WM8978_BIT_BOOSTENR |
    WM8978_BIT_LOUT1EN | WM8978_BIT_ROUT1EN);

  /* ---- R3: Power Management 3 ----
   * DACENL=1, DACENR=1 (enable DAC)
   * LMIXEN=1, RMIXEN=1 (enable output mixers)
   * LOUT2EN=1, ROUT2EN=1 (enable speaker/line out 2)
   * OUT3EN=1, OUT4EN=1 (enable OUT3/OUT4)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_POWER_MANAGEMENT_3,
    WM8978_BIT_DACENL | WM8978_BIT_DACENR |
    WM8978_BIT_LMIXEN | WM8978_BIT_RMIXEN |
    WM8978_BIT_LOUT2EN | WM8978_BIT_ROUT2EN |
    WM8978_BIT_OUT3EN | WM8978_BIT_OUT4EN);

  /* ---- R4: Audio Interface 1 ----
   * FMT=10 (I2S format)
   * WL=00 (16-bit word length, matching STM32 config)
   * BCP=0, LRP=0 (normal polarity)
   * MS=0 (slave mode - STM32 is master)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_AUDIO_INTERFACE_1,
    WM8978_FMT_I2S | WM8978_WL_16BIT);

  /* ---- R6: Clocking 1 ----
   * CLKSEL=1 (use PLL output as SYSCLK source)
   * MCLKDIV=010 (divide MCLK by 2)
   * BCLKDIV=000 (BCLK = SYSCLK)
   * MS=0 (slave mode)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_CLOCKING_1,
    WM8978_BIT_CLKSEL | WM8978_MCLKDIV_2);

  /* ---- R7: Clocking 2 ----
   * SR=011 (approximate 32kHz sample rate for digital filters)
   * SLOWCLKEN=0
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_CLOCKING_2,
    WM8978_SR_32K);

  /* ---- R32-R36: PLL Configuration ----
   * Generate exact SYSCLK = 256 * 32000 = 8.192 MHz from MCLK
   * MCLK = 51.2 / 6 = 8.5333 MHz (with PLLI2S=51.2MHz, MCLKDIV=2 → ÷6)
   *
   * WM8978 PLL: f_OUT = (f_IN / 2^R) × (N + K/65536)
   * Need: 8.192 = (8.5333 / 2^1) × (1 + K/65536)
   *       8.192 = 4.2667 × (1 + K/65536)
   *       (1 + K/65536) = 1.92
   *       K = 0.92 × 65536 = 60293
   * R32: PLLN = 1
   * R33: PLLK[23:18] = 60293 >> 18 = 0
   * R34: PLLK[17:9] = (60293 >> 9) & 0x1FF = 117
   * R35: PLLK[8:0] = 60293 & 0x1FF = 485 → but only 8 bits, so 485-256=229... wait
   * Let me recalculate:
   * K = 60293 = 0xEB25
   * K bits [23:16] = 0x00
   * K bits [15:8]  = 0xEB
   * K bits [7:0]   = 0x25
   *
   * R33 (PLL K1): bits [8:6] = PLLK[23:18] = 0x00, rest reserved
   * R34 (PLL K2): bits [8:0] = PLLK[17:9]
   *   PLLK[17:9] = (60293 >> 9) & 0x1FF = 117 = 0x75
   * R35 (PLL K3): bits [8:0] = PLLK[8:0]
   *   PLLK[8:0] = 60293 & 0x1FF = 37 ... wait
   *
   * 60293 in binary: 1110 1011 0010 0101
   * Bit 23-18: 000000
   * Bit 17-9:  0 1110 1011 0 = 0x075 ... let me recompute
   * 60293 = 0xEB25 = 0b 1110 1011 0010 0101 (16 bits)
   * As 24-bit: 0x00EB25 = 0b 0000 0000 1110 1011 0010 0101
   * Bit 23-18: 000000 → R33[5:0] = 0
   * Bit 17-9:  00 1110 1011 = 0x0EB → R34[8:0] = 0x0EB = 235
   * Bit 8-0:   010 0101 = 0x25 → R35[8:0] = 0x025 = 37
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_PLL_N, 0x0001);     /* PLLN=1 */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_PLL_K_1, 0x0000);  /* PLLK[23:18]=0 */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_PLL_K_2, 0x00EB);  /* PLLK[17:9]=235 */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_PLL_K_3, 0x0025);  /* PLLK[8:0]=37 */

  /* ---- R36: PLL Control ----
   * PLLEN=1 (enable PLL)
   * PLLR=1 (post-divider = 2)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_PLL_CONTROL,
    0x0001 | (1 << 4));  /* PLLEN=1, PLLR[3:0]=1 */

  /* ---- R44: Input Control ----
   * Connect L2/R2 directly to BOOST stage (bypass PGA)
   * L2_2INPPGA=0, R2_2INPPGA=0 (don't connect to PGA)
   * Disconnect default LIP/LIN/RIP/RIN connections
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_INPUT_CONTROL, 0x0000);

  /* ---- R45/R46: Input PGA Volume ----
   * PGA not used for guitar, set to minimum/mute
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_INPUT_PGA, 0x0040);  /* Mute */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_INPUT_PGA, 0x0040); /* Mute */

  /* ---- R47: Left ADC Boost ----
   * PGABOOSTL=0 (no PGA boost, PGA not used)
   * L2_2BOOSTVOL=100 (0dB gain from L2 pin to boost stage)
   * AUXL2BOOSTVOL=000 (no AUX input)
   *
   * Guitar 900mVpp = 318mVrms → 0dB gain → -10dBFS, good headroom
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_ADC_BOOST,
    (0x04 << 4));  /* L2_2BOOSTVOL=100 (0dB) */

  /* ---- R48: Right ADC Boost ----
   * PGABOOSTR=0
   * R2_2BOOSTVOL=100 (0dB gain from R2 pin)
   * AUXR2BOOSTVOL=000 (no AUX input)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_ADC_BOOST,
    (0x04 << 4));  /* R2_2BOOSTVOL=100 (0dB) */

  /* ---- R49: Output Control ----
   * TSDEN=1 (thermal shutdown enabled)
   * SPKBOOST=0 (speaker output 1x gain)
   * OUT3BOOST=0, OUT4BOOST=0 (1x gain, DC=AVDD/2)
   * VROI=0 (output impedance 1kΩ)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_OUTPUT_CONTROL,
    WM8978_BIT_TSDEN);

  /* ---- R50: Left Output Mixer ----
   * DACL2LMIX=1 (DAC to left output mixer)
   * BYPL2LMIX=0 (no bypass to mixer)
   * AUXL2LMIX=0 (no AUX to mixer)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_MIXER_CONTROL,
    WM8978_BIT_LD2LMIX);

  /* ---- R51: Right Output Mixer ----
   * DACR2RMIX=1 (DAC to right output mixer)
   * BYPR2RMIX=0 (no bypass to mixer)
   * AUXR2RMIX=0 (no AUX to mixer)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_MIXER_CONTROL,
    WM8978_BIT_LD2LMIX);

  /* ---- R52: Left Headphone Volume ----
   * Volume = 0dB → 111001 = 57
   * ZCD=0 (no zero cross detect for faster response)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_HP_VOL, 57);

  /* ---- R53: Right Headphone Volume ----
   * Volume = 0dB + HPVU=1 (update both channels)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_HP_VOL, 57 | WM8978_BIT_HPVU);

  /* ---- R54: Left Speaker Volume ----
   * Volume = 0dB → 111001 = 57
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_SPK_VOL, 57);

  /* ---- R55: Right Speaker Volume ----
   * Volume = 0dB + SPKVU=1 (update both channels)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_SPK_VOL, 57 | WM8978_BIT_SPKVU);

  /* ---- R56: OUT3 Mixer ----
   * LDAC2OUT3=1 (left DAC direct to OUT3)
   * OUT3MUTE=0 (output enabled)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_OUT3_MIXER_CONTROL,
    WM8978_BIT_LDAC2OUT3);

  /* ---- R57: OUT4 Mixer ----
   * RDAC2OUT4=1 (right DAC direct to OUT4)
   * OUT4MUTE=0 (output enabled)
   * HALFSIG=0 (normal output, not attenuated)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_OUT4_MIXER_CONTROL,
    WM8978_BIT_RDAC2OUT4);

  /* ---- R10: DAC Control ----
   * AMUTE=0 (disable auto-mute, continuous output)
   * DACOSR128=1 (128x oversampling for best quality)
   * DACPOL=0 (normal polarity)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_DAC_CONTROL,
    WM8978_BIT_DACOSR128);

  /* ---- R14: ADC Control ----
   * ADCOSR128=1 (128x oversampling for best quality)
   * ADCPOL=0 (normal polarity)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_ADC_CONTROL,
    WM8978_BIT_ADCOSR128);

  /* ---- R11/R12: DAC Digital Volume ----
   * Set to 0xFF (0dB, full scale) - master volume controlled by DSP
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_DAC_VOL, 0x00FF);
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_DAC_VOL, 0x00FF);

  /* ---- R15/R16: ADC Digital Volume ----
   * 0dB → 0xC3 (195)
   */
  ret += WM8978_WriteRegister(pObj, WM8978_REG_LEFT_ADC_VOL, 0x00C3);
  ret += WM8978_WriteRegister(pObj, WM8978_REG_RIGHT_ADC_VOL, 0x00C3);

  if (ret != WM8978_OK)
  {
    return WM8978_ERROR;
  }

  pObj->IsInitialized = 1;

  return WM8978_OK;
}

/**
  * @}
  */

/**
  * @}
  */
