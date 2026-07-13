#ifndef TFT_H
#define TFT_H

#include <stdint.h>

void TFT_Init(void);
void TFT_FillScreen(uint16_t color);
void TFT_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color);
void TFT_DrawStr(uint16_t x, uint16_t y, const char *s, uint16_t color);
void TFT_Test(void);

#endif
