/**
  ******************************************************************************
  * @file    wm8978_reg.h
  * @brief   WM8978 Audio Codec Register Definitions
  *          Based on WM8978 datasheet Rev 4.5, October 2011
  ******************************************************************************
  */

#ifndef WM8978_REG_H
#define WM8978_REG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/

/* R0 (00h) - Software Reset */
#define WM8978_REG_SOFTWARE_RESET        ((uint8_t)0x00)

/* R1 (01h) - Power Management 1 */
#define WM8978_REG_POWER_MANAGEMENT_1    ((uint8_t)0x01)

/* R2 (02h) - Power Management 2 */
#define WM8978_REG_POWER_MANAGEMENT_2    ((uint8_t)0x02)

/* R3 (03h) - Power Management 3 */
#define WM8978_REG_POWER_MANAGEMENT_3    ((uint8_t)0x03)

/* R4 (04h) - Audio Interface 1 */
#define WM8978_REG_AUDIO_INTERFACE_1     ((uint8_t)0x04)

/* R5 (05h) - Audio Interface 2 */
#define WM8978_REG_AUDIO_INTERFACE_2     ((uint8_t)0x05)

/* R6 (06h) - Clocking 1 */
#define WM8978_REG_CLOCKING_1            ((uint8_t)0x06)

/* R7 (07h) - Clocking 2 */
#define WM8978_REG_CLOCKING_2            ((uint8_t)0x07)

/* R8 (08h) - GPIO Control */
#define WM8978_REG_GPIO_CONTROL          ((uint8_t)0x08)

/* R9 (09h) - Jack Detect Control 1 */
#define WM8978_REG_JACK_DETECT_1         ((uint8_t)0x09)

/* R10 (0Ah) - DAC Control */
#define WM8978_REG_DAC_CONTROL           ((uint8_t)0x0A)

/* R11 (0Bh) - Left DAC Digital Volume */
#define WM8978_REG_LEFT_DAC_VOL          ((uint8_t)0x0B)

/* R12 (0Ch) - Right DAC Digital Volume */
#define WM8978_REG_RIGHT_DAC_VOL         ((uint8_t)0x0C)

/* R13 (0Dh) - Jack Detect Control 2 */
#define WM8978_REG_JACK_DETECT_2         ((uint8_t)0x0D)

/* R14 (0Eh) - ADC Control */
#define WM8978_REG_ADC_CONTROL           ((uint8_t)0x0E)

/* R15 (0Fh) - Left ADC Digital Volume */
#define WM8978_REG_LEFT_ADC_VOL          ((uint8_t)0x0F)

/* R16 (10h) - Right ADC Digital Volume */
#define WM8978_REG_RIGHT_ADC_VOL         ((uint8_t)0x10)

/* R17 (11h) - EQ1 - Low Shelf */
#define WM8978_REG_EQ1_LOW_SHELF         ((uint8_t)0x11)

/* R18 (12h) - EQ2 - Peaking */
#define WM8978_REG_EQ2_PEAKING           ((uint8_t)0x12)

/* R19 (13h) - EQ3 - Peaking */
#define WM8978_REG_EQ3_PEAKING           ((uint8_t)0x13)

/* R20 (14h) - EQ4 - Peaking */
#define WM8978_REG_EQ4_PEAKING           ((uint8_t)0x14)

/* R21 (15h) - EQ5 - High Shelf */
#define WM8978_REG_EQ5_HIGH_SHELF        ((uint8_t)0x15)

/* R22 (16h) - DAC Limitter 1 */
#define WM8978_REG_DAC_LIMITER_1         ((uint8_t)0x16)

/* R23 (17h) - DAC Limitter 2 */
#define WM8978_REG_DAC_LIMITER_2         ((uint8_t)0x17)

/* R24 (18h) - Notch Filter 1 */
#define WM8978_REG_NOTCH_FILTER_1        ((uint8_t)0x18)

/* R25 (19h) - Notch Filter 2 */
#define WM8978_REG_NOTCH_FILTER_2        ((uint8_t)0x19)

/* R26 (1Ah) - Notch Filter 3 */
#define WM8978_REG_NOTCH_FILTER_3        ((uint8_t)0x1A)

/* R27 (1Bh) - Notch Filter 4 */
#define WM8978_REG_NOTCH_FILTER_4        ((uint8_t)0x1B)

/* R28 (1Ch) - ALC Control 1 */
#define WM8978_REG_ALC_CONTROL_1         ((uint8_t)0x1C)

/* R29 (1Dh) - ALC Control 2 */
#define WM8978_REG_ALC_CONTROL_2         ((uint8_t)0x1D)

/* R30 (1Eh) - ALC Control 3 */
#define WM8978_REG_ALC_CONTROL_3         ((uint8_t)0x1E)

/* R31 (1Fh) - Noise Gate */
#define WM8978_REG_NOISE_GATE            ((uint8_t)0x1F)

/* R32 (20h) - PLL N */
#define WM8978_REG_PLL_N                 ((uint8_t)0x20)

/* R33 (21h) - PLL K 1 */
#define WM8978_REG_PLL_K_1               ((uint8_t)0x21)

/* R34 (22h) - PLL K 2 */
#define WM8978_REG_PLL_K_2               ((uint8_t)0x22)

/* R35 (23h) - PLL K 3 */
#define WM8978_REG_PLL_K_3               ((uint8_t)0x23)

/* R36 (24h) - PLL Control */
#define WM8978_REG_PLL_CONTROL           ((uint8_t)0x24)

/* R37 (25h) - BCLK/LRC Control */
#define WM8978_REG_BCLK_LRC_CONTROL      ((uint8_t)0x25)

/* R38 (26h) - Left Speaker Mixer */
#define WM8978_REG_LEFT_SPEAKER_MIXER    ((uint8_t)0x26)

/* R39 (27h) - Right Speaker Mixer */
#define WM8978_REG_RIGHT_SPEAKER_MIXER   ((uint8_t)0x27)

/* R40 (28h) - Left Output Mixer */
#define WM8978_REG_LEFT_OUTPUT_MIXER     ((uint8_t)0x28)

/* R41 (29h) - Right Output Mixer */
#define WM8978_REG_RIGHT_OUTPUT_MIXER    ((uint8_t)0x29)

/* R42 (2Ah) - Mono Output Mixer */
#define WM8978_REG_MONO_OUTPUT_MIXER     ((uint8_t)0x2A)

/* R43 (2Bh) - Beep Control */
#define WM8978_REG_BEEP_CONTROL          ((uint8_t)0x2B)

/* R44 (2Ch) - Input Control */
#define WM8978_REG_INPUT_CONTROL         ((uint8_t)0x2C)

/* R45 (2Dh) - Left Input PGA */
#define WM8978_REG_LEFT_INPUT_PGA        ((uint8_t)0x2D)

/* R46 (2Eh) - Right Input PGA */
#define WM8978_REG_RIGHT_INPUT_PGA       ((uint8_t)0x2E)

/* R47 (2Fh) - Left ADC Boost */
#define WM8978_REG_LEFT_ADC_BOOST        ((uint8_t)0x2F)

/* R48 (30h) - Right ADC Boost */
#define WM8978_REG_RIGHT_ADC_BOOST       ((uint8_t)0x30)

/* R49 (31h) - Output Control */
#define WM8978_REG_OUTPUT_CONTROL        ((uint8_t)0x31)

/* R50 (32h) - Left Mixer Control */
#define WM8978_REG_LEFT_MIXER_CONTROL    ((uint8_t)0x32)

/* R51 (33h) - Right Mixer Control */
#define WM8978_REG_RIGHT_MIXER_CONTROL   ((uint8_t)0x33)

/* R52 (34h) - Left Headphone Out Volume */
#define WM8978_REG_LEFT_HP_VOL           ((uint8_t)0x34)

/* R53 (35h) - Right Headphone Out Volume */
#define WM8978_REG_RIGHT_HP_VOL          ((uint8_t)0x35)

/* R54 (36h) - Left Speaker Out Volume */
#define WM8978_REG_LEFT_SPK_VOL          ((uint8_t)0x36)

/* R55 (37h) - Right Speaker Out Volume */
#define WM8978_REG_RIGHT_SPK_VOL         ((uint8_t)0x37)

/* R56 (38h) - OUT3 Mixer Control */
#define WM8978_REG_OUT3_MIXER_CONTROL    ((uint8_t)0x38)

/* R57 (39h) - OUT4 Mixer Control */
#define WM8978_REG_OUT4_MIXER_CONTROL    ((uint8_t)0x39)


/******************************************************************************/
/****************************** REGISTER BIT MASKS ****************************/
/******************************************************************************/

/* R1 - Power Management 1 */
#define WM8978_BIT_BUFDCOPEN             ((uint16_t)(1 << 8))
#define WM8978_BIT_OUT4MIXEN             ((uint16_t)(1 << 7))
#define WM8978_BIT_OUT3MIXEN             ((uint16_t)(1 << 6))
#define WM8978_BIT_PLLEN                 ((uint16_t)(1 << 5))
#define WM8978_BIT_MICBEN                ((uint16_t)(1 << 4))
#define WM8978_BIT_BIASEN                ((uint16_t)(1 << 3))
#define WM8978_BIT_BUFIOEN               ((uint16_t)(1 << 2))
#define WM8978_MASK_VMIDSEL              ((uint16_t)(0x03 << 0))
#define WM8978_VMIDSEL_OFF               ((uint16_t)(0x00 << 0))
#define WM8978_VMIDSEL_75K               ((uint16_t)(0x01 << 0))
#define WM8978_VMIDSEL_300K              ((uint16_t)(0x02 << 0))
#define WM8978_VMIDSEL_5K                ((uint16_t)(0x03 << 0))

/* R2 - Power Management 2 */
#define WM8978_BIT_ROUT1EN               ((uint16_t)(1 << 8))
#define WM8978_BIT_LOUT1EN               ((uint16_t)(1 << 7))
#define WM8978_BIT_SLEEP                 ((uint16_t)(1 << 6))
#define WM8978_BIT_BOOSTENR              ((uint16_t)(1 << 5))
#define WM8978_BIT_BOOSTENL              ((uint16_t)(1 << 4))
#define WM8978_BIT_INPPGAENR             ((uint16_t)(1 << 3))
#define WM8978_BIT_INPPGAENL             ((uint16_t)(1 << 2))
#define WM8978_BIT_ADCENR                ((uint16_t)(1 << 1))
#define WM8978_BIT_ADCENL                ((uint16_t)(1 << 0))

/* R3 - Power Management 3 */
#define WM8978_BIT_OUT4EN                ((uint16_t)(1 << 8))
#define WM8978_BIT_OUT3EN                ((uint16_t)(1 << 7))
#define WM8978_BIT_LOUT2EN               ((uint16_t)(1 << 6))
#define WM8978_BIT_ROUT2EN               ((uint16_t)(1 << 5))
#define WM8978_BIT_RMIXEN                ((uint16_t)(1 << 3))
#define WM8978_BIT_LMIXEN                ((uint16_t)(1 << 2))
#define WM8978_BIT_DACENR                ((uint16_t)(1 << 1))
#define WM8978_BIT_DACENL                ((uint16_t)(1 << 0))

/* R4 - Audio Interface 1 */
#define WM8978_BIT_BCP                   ((uint16_t)(1 << 8))
#define WM8978_BIT_LRP                   ((uint16_t)(1 << 7))
#define WM8978_MASK_WL                   ((uint16_t)(0x03 << 5))
#define WM8978_WL_16BIT                  ((uint16_t)(0x00 << 5))
#define WM8978_WL_20BIT                  ((uint16_t)(0x01 << 5))
#define WM8978_WL_24BIT                  ((uint16_t)(0x02 << 5))
#define WM8978_WL_32BIT                  ((uint16_t)(0x03 << 5))
#define WM8978_MASK_FMT                  ((uint16_t)(0x03 << 3))
#define WM8978_FMT_RIGHT_JUSTIFIED       ((uint16_t)(0x00 << 3))
#define WM8978_FMT_LEFT_JUSTIFIED        ((uint16_t)(0x01 << 3))
#define WM8978_FMT_I2S                   ((uint16_t)(0x02 << 3))
#define WM8978_FMT_PCM_DSP              ((uint16_t)(0x03 << 3))
#define WM8978_BIT_DACLRSWAP             ((uint16_t)(1 << 2))
#define WM8978_BIT_ADCLRSWAP             ((uint16_t)(1 << 1))
#define WM8978_BIT_MONO                  ((uint16_t)(1 << 0))

/* R6 - Clocking 1 */
#define WM8978_BIT_CLKSEL                ((uint16_t)(1 << 8))
#define WM8978_MASK_MCLKDIV              ((uint16_t)(0x07 << 5))
#define WM8978_MCLKDIV_1                 ((uint16_t)(0x00 << 5))
#define WM8978_MCLKDIV_1_5               ((uint16_t)(0x01 << 5))
#define WM8978_MCLKDIV_2                 ((uint16_t)(0x02 << 5))
#define WM8978_MCLKDIV_3                 ((uint16_t)(0x03 << 5))
#define WM8978_MCLKDIV_4                 ((uint16_t)(0x04 << 5))
#define WM8978_MCLKDIV_6                 ((uint16_t)(0x05 << 5))
#define WM8978_MCLKDIV_8                 ((uint16_t)(0x06 << 5))
#define WM8978_MCLKDIV_12                ((uint16_t)(0x07 << 5))
#define WM8978_MASK_BCLKDIV              ((uint16_t)(0x07 << 2))
#define WM8978_BCLKDIV_1                 ((uint16_t)(0x00 << 2))
#define WM8978_BCLKDIV_2                 ((uint16_t)(0x01 << 2))
#define WM8978_BCLKDIV_4                 ((uint16_t)(0x02 << 2))
#define WM8978_BCLKDIV_8                 ((uint16_t)(0x03 << 2))
#define WM8978_BCLKDIV_16                ((uint16_t)(0x04 << 2))
#define WM8978_BCLKDIV_32                ((uint16_t)(0x05 << 2))
#define WM8978_BIT_MS                    ((uint16_t)(1 << 0))

/* R7 - Clocking 2 */
#define WM8978_MASK_SR                   ((uint16_t)(0x07 << 1))
#define WM8978_SR_48K                    ((uint16_t)(0x00 << 1))
#define WM8978_SR_32K                    ((uint16_t)(0x01 << 1))
#define WM8978_SR_24K                    ((uint16_t)(0x02 << 1))
#define WM8978_SR_16K                    ((uint16_t)(0x03 << 1))
#define WM8978_SR_12K                    ((uint16_t)(0x04 << 1))
#define WM8978_SR_8K                     ((uint16_t)(0x05 << 1))
#define WM8978_BIT_SLOWCLKEN             ((uint16_t)(1 << 0))

/* R10 - DAC Control */
#define WM8978_BIT_DACOSR128             ((uint16_t)(1 << 3))
#define WM8978_BIT_AMUTE                 ((uint16_t)(1 << 2))
#define WM8978_BIT_DACPOLR               ((uint16_t)(1 << 1))
#define WM8978_BIT_DACPOL                ((uint16_t)(1 << 0))

/* R14 - ADC Control */
#define WM8978_BIT_ADCOSR128             ((uint16_t)(1 << 3))
#define WM8978_BIT_ADCPOLR               ((uint16_t)(1 << 1))
#define WM8978_BIT_ADCPOL                ((uint16_t)(1 << 0))

/* R18 - EQ2 (EQ1/2/3/4/5 gain) */
#define WM8978_MASK_EQ1C                 ((uint16_t)(0x03 << 5))
#define WM8978_MASK_EQ1G                  ((uint16_t)(0x1F << 0))

/* R33/R34/R35 - PLL K */
#define WM8978_MASK_PLLK_23_18           ((uint16_t)(0x3F << 0))

/* R44 - Input Control */
#define WM8978_BIT_LIN2INPPGA            ((uint16_t)(1 << 7))
#define WM8978_BIT_LIP2INPGA             ((uint16_t)(1 << 6))
#define WM8978_BIT_RIN2INPPGA            ((uint16_t)(1 << 5))
#define WM8978_BIT_RIP2INPGA             ((uint16_t)(1 << 4))
#define WM8978_BIT_LIN12INPPGA           ((uint16_t)(1 << 3))
#define WM8978_BIT_LIP1INPGA             ((uint16_t)(1 << 2))
#define WM8978_BIT_RIN12INPPGA           ((uint16_t)(1 << 1))
#define WM8978_BIT_RIP1INPGA             ((uint16_t)(1 << 0))

/* R47 - Left ADC Boost */
#define WM8978_MASK_PGABOOSTL            ((uint16_t)(0x01 << 8))
#define WM8978_MASK_L2_2BOOSTVOL         ((uint16_t)(0x07 << 4))
#define WM8978_MASK_AUXL2BOOSTVOL        ((uint16_t)(0x07 << 0))

/* R48 - Right ADC Boost */
#define WM8978_MASK_PGABOOSTR            ((uint16_t)(0x01 << 8))
#define WM8978_MASK_R2_2BOOSTVOL         ((uint16_t)(0x07 << 4))
#define WM8978_MASK_AUXR2BOOSTVOL        ((uint16_t)(0x07 << 0))

/* R49 - Output Control */
#define WM8978_BIT_OUT4BOOST             ((uint16_t)(1 << 4))
#define WM8978_BIT_OUT3BOOST             ((uint16_t)(1 << 3))
#define WM8978_BIT_SPKBOOST              ((uint16_t)(1 << 2))
#define WM8978_BIT_TSDEN                 ((uint16_t)(1 << 1))
#define WM8978_BIT_VROI                  ((uint16_t)(1 << 0))

/* R50/R51 - Left/Right Mixer Control */
#define WM8978_BIT_LD2LMIX               ((uint16_t)(1 << 0))
#define WM8978_BIT_LI2LMIX               ((uint16_t)(1 << 1))
#define WM8978_MASK_AUXLMIXVOL            ((uint16_t)(0x07 << 2))

/* R52/R53 - Left/Right Headphone Volume */
#define WM8978_MASK_HPVOL                ((uint16_t)(0x3F << 0))
#define WM8978_BIT_HPVU                  ((uint16_t)(1 << 8))
#define WM8978_BIT_ZCD                   ((uint16_t)(1 << 7))

/* R54/R55 - Left/Right Speaker Volume */
#define WM8978_MASK_SPKVOL               ((uint16_t)(0x3F << 0))
#define WM8978_BIT_SPKVU                 ((uint16_t)(1 << 8))

/* R56 - OUT3 Mixer */
#define WM8978_BIT_OUT3MUTE              ((uint16_t)(1 << 6))
#define WM8978_BIT_OUT4_2OUT3            ((uint16_t)(1 << 3))
#define WM8978_BIT_BYPL2OUT3             ((uint16_t)(1 << 2))
#define WM8978_BIT_LMIX2OUT3             ((uint16_t)(1 << 1))
#define WM8978_BIT_LDAC2OUT3             ((uint16_t)(1 << 0))

/* R57 - OUT4 Mixer */
#define WM8978_BIT_OUT4MUTE              ((uint16_t)(1 << 6))
#define WM8978_BIT_HALFSIG               ((uint16_t)(1 << 5))
#define WM8978_BIT_LMIX2OUT4             ((uint16_t)(1 << 4))
#define WM8978_BIT_LDAC2OUT4             ((uint16_t)(1 << 3))
#define WM8978_BIT_BYPR2OUT4             ((uint16_t)(1 << 2))
#define WM8978_BIT_RMIX2OUT4             ((uint16_t)(1 << 1))
#define WM8978_BIT_RDAC2OUT4             ((uint16_t)(1 << 0))


/******************************************************************************/
/****************************** REGISTER ACCESS *******************************/
/******************************************************************************/

/* I2C Address: AD0 pin low = 0x1A, AD0 pin high = 0x1B */
#define WM8978_I2C_ADDR_LOW              ((uint8_t)0x1A)
#define WM8978_I2C_ADDR_HIGH             ((uint8_t)0x1B)
#define WM8978_I2C_ADDR_DEFAULT          WM8978_I2C_ADDR_LOW

/* Total number of registers */
#define WM8978_REGISTER_COUNT            ((uint8_t)58)


#ifdef __cplusplus
}
#endif

#endif /* WM8978_REG_H */
