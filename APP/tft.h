#ifndef TFT_H
#define TFT_H

#include <stdint.h>

void TFT_Init(void);
void TFT_FillScreen(uint16_t color);
void TFT_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color);
void TFT_DrawStr(uint16_t x, uint16_t y, const char *s, uint16_t color);
/* 反显(带背景色)文本 */
void TFT_DrawStrEx(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg);
/* 实心矩形 */
void TFT_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void TFT_Test(void);

#endif
