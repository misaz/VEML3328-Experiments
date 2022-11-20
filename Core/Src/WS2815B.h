#ifndef SRC_WS2815B_H_
#define SRC_WS2815B_H_

#include <stddef.h>
#include <stdint.h>

#define WS2815B_MAX_LEDS 4

void WS2815B_Init();
void WS2815B_SetColor(size_t led, uint8_t r, uint8_t g, uint8_t b);
void WS2815B_SendsData();

#endif
