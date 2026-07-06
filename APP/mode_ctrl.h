#ifndef MODE_CTRL_H
#define MODE_CTRL_H

void ModeCtrl_Init(void);
void ModeCtrl_Poll(float pot_value);

extern volatile float g_master_volume;

#endif
