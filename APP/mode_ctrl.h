#ifndef MODE_CTRL_H
#define MODE_CTRL_H

#include <stdint.h>

void ModeCtrl_Init(void);
void ModeCtrl_Poll(int16_t enc_delta);

extern volatile float g_master_volume;

#endif
