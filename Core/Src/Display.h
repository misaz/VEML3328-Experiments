#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include <stdint.h>

void Display_Init();
void Display_SetPixelBuffer(int x, int row, uint8_t value);
void Display_SendBuffer();
void Display_Clear();
int Display_PrintChar(int x, int row, char ch);
int Display_PrintString(int x, int row, char* str);

#endif
