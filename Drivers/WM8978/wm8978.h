/**
  ******************************************************************************
  * @file    wm8978.h
  * @brief   WM8978 Audio Codec Driver
  *          Stereo CODEC with Speaker Driver
  *          Based on WM8978 datasheet Rev 4.5 and STM32 WM8994 driver style
  ******************************************************************************
  */

#ifndef WM8978_H
#define WM8978_H

/* Includes ------------------------------------------------------------------*/
#include "wm8978_reg.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup WM8978
  * @{
  */

/** @defgroup WM8978_Exported_Types Exported Types
  * @{
  */

/* Function pointer types for I2C communication */
typedef int32_t (*WM8978_Init_Func)(void);
typedef int32_t (*WM8978_DeInit_Func)(void);
typedef int32_t (*WM8978_GetTick_Func)(void);
typedef int32_t (*WM8978_Delay_Func)(uint32_t);
typedef int32_t (*WM8978_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*WM8978_ReadReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);

/* IO context structure */
typedef struct
{
  WM8978_Init_Func          Init;
  WM8978_DeInit_Func        DeInit;
  uint16_t                  Address;
  WM8978_WriteReg_Func      WriteReg;
  WM8978_ReadReg_Func       ReadReg;
  WM8978_GetTick_Func       GetTick;
} WM8978_IO_t;

/* Main codec object structure */
typedef struct
{
  WM8978_IO_t         IO;
  uint16_t            RegCache[WM8978_REGISTER_COUNT]; /* Register cache */
  uint8_t             IsInitialized;
} WM8978_Object_t;

/* Initialization structure */
typedef struct
{
  uint32_t   InputDevice;
  uint32_t   OutputDevice;
  uint32_t   Frequency;
  uint32_t   Resolution;
  uint32_t   Volume;
} WM8978_Init_t;

/* Driver structure (virtual function table) */
typedef struct
{
  int32_t (*Init)(WM8978_Object_t*, WM8978_Init_t*);
  int32_t (*DeInit)(WM8978_Object_t*);
  int32_t (*ReadID)(WM8978_Object_t*, uint32_t*);
  int32_t (*Play)(WM8978_Object_t*);
  int32_t (*Pause)(WM8978_Object_t*);
  int32_t (*Resume)(WM8978_Object_t*);
  int32_t (*Stop)(WM8978_Object_t*, uint32_t);
  int32_t (*SetFrequency)(WM8978_Object_t*, uint32_t);
  int32_t (*GetFrequency)(WM8978_Object_t*, uint32_t*);
  int32_t (*SetVolume)(WM8978_Object_t*, uint32_t, uint8_t);
  int32_t (*GetVolume)(WM8978_Object_t*, uint32_t, uint8_t*);
  int32_t (*SetMute)(WM8978_Object_t*, uint32_t);
  int32_t (*SetOutputMode)(WM8978_Object_t*, uint32_t);
  int32_t (*SetResolution)(WM8978_Object_t*, uint32_t);
  int32_t (*GetResolution)(WM8978_Object_t*, uint32_t*);
  int32_t (*SetProtocol)(WM8978_Object_t*, uint32_t);
  int32_t (*GetProtocol)(WM8978_Object_t*, uint32_t*);
  int32_t (*Reset)(WM8978_Object_t*);
} WM8978_Drv_t;

/**
  * @}
  */

/** @defgroup WM8978_Exported_Constants Exported Constants
  * @{
  */

/* Status codes */
#define WM8978_OK                ((int32_t)0)
#define WM8978_ERROR             ((int32_t)-1)

/* Audio Input Device */
#define WM8978_IN_NONE           ((uint32_t)0x0000)
#define WM8978_IN_MIC1           ((uint32_t)0x0001)  /* LINPUT1/RINPUT1 */
#define WM8978_IN_MIC2           ((uint32_t)0x0002)  /* LINPUT2/RINPUT2 via PGA */
#define WM8978_IN_LINE1          ((uint32_t)0x0003)  /* LINPUT1/RINPUT1 direct */
#define WM8978_IN_LINE2          ((uint32_t)0x0004)  /* LINPUT2/RINPUT2 direct */
#define WM8978_IN_AUX            ((uint32_t)0x0005)  /* AUXL/AUXR */

/* Audio Output Device */
#define WM8978_OUT_NONE          ((uint32_t)0x0000)
#define WM8978_OUT_SPEAKER       ((uint32_t)0x0001)  /* LOUT2/ROUT2 */
#define WM8978_OUT_HEADPHONE     ((uint32_t)0x0002)  /* LOUT1/ROUT1 */
#define WM8978_OUT_BOTH          ((uint32_t)0x0003)  /* Both HP and Speaker */
#define WM8978_OUT_LINE          ((uint32_t)0x0004)  /* OUT3/OUT4 line output */

/* AUDIO FREQUENCY */
#define WM8978_FREQUENCY_48K     ((uint32_t)48000)
#define WM8978_FREQUENCY_44K     ((uint32_t)44100)
#define WM8978_FREQUENCY_32K     ((uint32_t)32000)
#define WM8978_FREQUENCY_24K     ((uint32_t)24000)
#define WM8978_FREQUENCY_22K     ((uint32_t)22050)
#define WM8978_FREQUENCY_16K     ((uint32_t)16000)
#define WM8978_FREQUENCY_12K     ((uint32_t)12000)
#define WM8978_FREQUENCY_11K     ((uint32_t)11025)
#define WM8978_FREQUENCY_8K      ((uint32_t)8000)

/* AUDIO RESOLUTION (Word Length) */
#define WM8978_RESOLUTION_16b    ((uint32_t)0x00)
#define WM8978_RESOLUTION_20b    ((uint32_t)0x01)
#define WM8978_RESOLUTION_24b    ((uint32_t)0x02)
#define WM8978_RESOLUTION_32b    ((uint32_t)0x03)

/* Codec stop options */
#define WM8978_PDWN_HW           ((uint32_t)0x00)
#define WM8978_PDWN_SW           ((uint32_t)0x01)

/* Volume Input Output selection */
#define VOLUME_INPUT             ((uint32_t)0)
#define VOLUME_OUTPUT            ((uint32_t)1)

/* MUTE commands */
#define WM8978_MUTE_ON           ((uint32_t)1)
#define WM8978_MUTE_OFF          ((uint32_t)0)

/* AUDIO PROTOCOL */
#define WM8978_PROTOCOL_R_JUSTIFIED  ((uint32_t)0x00)
#define WM8978_PROTOCOL_L_JUSTIFIED  ((uint32_t)0x01)
#define WM8978_PROTOCOL_I2S          ((uint32_t)0x02)
#define WM8978_PROTOCOL_DSP          ((uint32_t)0x03)

/* Volume conversion macros */
#define WM8978_VOL_OUT_INVERT(Volume)  ((uint8_t)(((Volume) * 100) / 63))
#define WM8978_VOL_IN_INVERT(Volume)   ((uint8_t)(((Volume) * 100) / 255))

/**
  * @}
  */

/** @addtogroup WM8978_Exported_Variables
  * @{
  */
extern WM8978_Drv_t WM8978_Driver;
/**
  * @}
  */

/** @addtogroup WM8978_Exported_Functions
  * @{
  */

/* Bus IO registration */
int32_t WM8978_RegisterBusIO(WM8978_Object_t *pObj, WM8978_IO_t *pIO);

/* High Layer codec functions */
int32_t WM8978_Init(WM8978_Object_t *pObj, WM8978_Init_t *pInit);
int32_t WM8978_DeInit(WM8978_Object_t *pObj);
int32_t WM8978_ReadID(WM8978_Object_t *pObj, uint32_t *Id);
int32_t WM8978_Play(WM8978_Object_t *pObj);
int32_t WM8978_Pause(WM8978_Object_t *pObj);
int32_t WM8978_Resume(WM8978_Object_t *pObj);
int32_t WM8978_Stop(WM8978_Object_t *pObj, uint32_t CodecPdwnMode);
int32_t WM8978_SetVolume(WM8978_Object_t *pObj, uint32_t InputOutput, uint8_t Volume);
int32_t WM8978_GetVolume(WM8978_Object_t *pObj, uint32_t InputOutput, uint8_t *Volume);
int32_t WM8978_SetMute(WM8978_Object_t *pObj, uint32_t Cmd);
int32_t WM8978_SetOutputMode(WM8978_Object_t *pObj, uint32_t Output);
int32_t WM8978_SetResolution(WM8978_Object_t *pObj, uint32_t Resolution);
int32_t WM8978_GetResolution(WM8978_Object_t *pObj, uint32_t *Resolution);
int32_t WM8978_SetProtocol(WM8978_Object_t *pObj, uint32_t Protocol);
int32_t WM8978_GetProtocol(WM8978_Object_t *pObj, uint32_t *Protocol);
int32_t WM8978_SetFrequency(WM8978_Object_t *pObj, uint32_t AudioFreq);
int32_t WM8978_GetFrequency(WM8978_Object_t *pObj, uint32_t *AudioFreq);
int32_t WM8978_Reset(WM8978_Object_t *pObj);

/* Extended codec functions */
int32_t WM8978_SetMICGain(WM8978_Object_t *pObj, uint8_t Gain);
int32_t WM8978_SetLINEINGain(WM8978_Object_t *pObj, uint8_t Gain);
int32_t WM8978_SetAUXGain(WM8978_Object_t *pObj, uint8_t Gain);
int32_t WM8978_SetEQ(WM8978_Object_t *pObj, uint8_t EQBand, uint8_t Freq, uint8_t Gain);
int32_t WM8978_Set3D(WM8978_Object_t *pObj, uint8_t Depth);
int32_t WM8978_SetEQ3DDirection(WM8978_Object_t *pObj, uint8_t Dir);

/* Guitar-specific initialization (L2/R2 input → HP + OUT3/OUT4 output) */
int32_t WM8978_Init_Guitar(WM8978_Object_t *pObj);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* WM8978_H */
