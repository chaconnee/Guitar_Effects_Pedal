#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#define SETTINGS_MAGIC  0x57574D38
#define IR_COUNT        3

typedef struct {
    uint32_t magic;
    float    amp_drive;
    float    rv_decay;
    float    rv_mix;
    float    rv_tone;
    uint8_t  ir_index;
    uint8_t  cab_bypassed;
    uint8_t  amp_bypassed;
    uint8_t  rv_bypassed;
    uint32_t crc;
} Settings_t;

void Settings_Load(Settings_t *s);
void Settings_Save(Settings_t *s);

#endif
