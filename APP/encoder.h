#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

void Enc_Init(void);

/* Returns signed delta since last call (positive=CW, negative=CCW) */
int16_t Enc_ReadDelta(void);

#endif
