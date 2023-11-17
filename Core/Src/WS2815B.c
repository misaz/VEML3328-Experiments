#include "WS2815B.h"
#include "stm32g0xx_hal.h"
#include <stdint.h>

#define WS2815B_RESTART_DELAY 20

static uint8_t transfer_data[WS2815B_RESTART_DELAY + WS2815B_MAX_LEDS * 9 + 1];
static uint8_t *leds_data = transfer_data + WS2815B_RESTART_DELAY;

extern SPI_HandleTypeDef hspi2;

void WS2815B_Init() {
	transfer_data[WS2815B_RESTART_DELAY + WS2815B_MAX_LEDS * 9] = 0x00;

	for (int i = 0; i < WS2815B_MAX_LEDS; i++) {
		WS2815B_SetColor(i, 0, 0, 0);
	}
}

void WS2815B_SetColor(size_t led, uint8_t r, uint8_t g, uint8_t b) {
	uint32_t newR = 0;
	uint32_t newG = 0;
	uint32_t newB = 0;

	for (int i = 0; i < 8; i++) {
		if (r & (1 << i)) {
			newR |= 0b110 << (3 * i);
		} else {
			newR |= 0b100 << (3 * i);
		}

		if (g & (1 << i)) {
			newG |= 0b110 << (3 * i);
		} else {
			newG |= 0b100 << (3 * i);
		}

		if (b & (1 << i)) {
			newB |= 0b110 << (3 * i);
		} else {
			newB |= 0b100 << (3 * i);
		}
	}

	leds_data[led * 9 + 0] = (uint8_t) ((newG & 0xFF0000) >> 16);
	leds_data[led * 9 + 1] = (uint8_t) ((newG & 0x00FF00) >> 8);
	leds_data[led * 9 + 2] = (uint8_t) ((newG & 0x0000FF) >> 0);

	leds_data[led * 9 + 3] = (uint8_t) ((newR & 0xFF0000) >> 16);
	leds_data[led * 9 + 4] = (uint8_t) ((newR & 0x00FF00) >> 8);
	leds_data[led * 9 + 5] = (uint8_t) ((newR & 0x0000FF) >> 0);

	leds_data[led * 9 + 6] = (uint8_t) ((newB & 0xFF0000) >> 16);
	leds_data[led * 9 + 7] = (uint8_t) ((newB & 0x00FF00) >> 8);
	leds_data[led * 9 + 8] = (uint8_t) ((newB & 0x0000FF) >> 0);
}

static int isCompleted = 0;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi) {
	isCompleted = 1;
}

void WS2815B_SendsData() {
	HAL_StatusTypeDef status;

	isCompleted = 0;

	GPIO_InitTypeDef miso;
	miso.Mode = GPIO_MODE_AF_PP;
	miso.Pin = GPIO_PIN_7;
	miso.Alternate = GPIO_AF5_SPI2;
	miso.Pull = GPIO_NOPULL;
	miso.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &miso);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);

	HAL_SPI_Init(&hspi2);

	GPIO_InitTypeDef clk;
	clk.Mode = GPIO_MODE_INPUT;
	clk.Pin = GPIO_PIN_0;
	clk.Alternate = 0;
	clk.Pull = GPIO_PULLUP;
	clk.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &clk);

	status = HAL_SPI_Transmit_DMA(&hspi2, transfer_data, sizeof(transfer_data));
	if (status) {
		__BKPT(0);
	}

	while (!isCompleted) {
		__WFI();
	}

	HAL_SPI_DeInit(&hspi2);

	miso.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &miso);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
}


