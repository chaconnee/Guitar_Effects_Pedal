#include "settings.h"
#include "stm32f4xx_hal.h"
#include <string.h>

#define SETTINGS_SECTOR  FLASH_SECTOR_7
#define SETTINGS_ADDR    0x08060000

static uint32_t CalcCRC(Settings_t *s)
{
    uint32_t *p = (uint32_t *)s;
    uint32_t xor = 0;
    for (int i = 0; i < (sizeof(Settings_t) - 4) / 4; i++)
        xor ^= p[i];
    return xor;
}

void Settings_Save(Settings_t *s)
{
    s->crc = CalcCRC(s);

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = SETTINGS_SECTOR;
    erase.NbSectors = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    uint32_t err;
    HAL_FLASHEx_Erase(&erase, &err);

    uint32_t *src = (uint32_t *)s;
    uint32_t addr = SETTINGS_ADDR;
    for (int i = 0; i < sizeof(Settings_t) / 4; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, src[i]);
        addr += 4;
    }

    HAL_FLASH_Lock();
}

void Settings_Load(Settings_t *s)
{
    Settings_t *flash = (Settings_t *)SETTINGS_ADDR;
    if (flash->magic == SETTINGS_MAGIC && flash->crc == CalcCRC(flash))
    {
        memcpy(s, flash, sizeof(Settings_t));
    }
    else
    {
        s->magic        = SETTINGS_MAGIC;
        s->amp_drive    = 0.8f;
        s->rv_decay     = 0.7f;
        s->rv_mix       = 0.3f;
        s->rv_tone      = 0.5f;
        s->ir_index     = 0;
        s->cab_bypassed = 0;
        s->amp_bypassed = 0;
        s->rv_bypassed  = 0;
    }
}
