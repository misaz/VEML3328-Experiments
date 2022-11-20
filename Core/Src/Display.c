#include "Display.h"
#include "stm32g0xx_hal.h"

extern I2C_HandleTypeDef hi2c2;

#define DISPLAY_I2C_ADDRESS 0x3C

static uint8_t fontDefinition[] = {
		0x0e, 0x11, 0x11, 0x0e, 0x12, 0x1f, 0x10, 0x12, 0x19, 0x15, 0x12, 0x11, 0x15, 0x15, 0x0a, 0x0c,
		0x0a, 0x09, 0x1f, 0x17, 0x15, 0x15, 0x0d, 0x0e, 0x15, 0x15, 0x08, 0x01, 0x01, 0x1d, 0x03, 0x0a,
		0x15, 0x15, 0x0a, 0x02, 0x15, 0x15, 0x0e, 0x1e, 0x05, 0x05, 0x1e, 0x1f, 0x15, 0x15, 0x0a, 0x0e,
		0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x0e, 0x1f, 0x15, 0x15, 0x1f, 0x05, 0x05, 0x0e, 0x11, 0x15,
		0x1d, 0x1f, 0x04, 0x04, 0x1f, 0x1f, 0x08, 0x10, 0x10, 0x0f, 0x1f, 0x04, 0x0a, 0x11, 0x1f, 0x10,
		0x10, 0x1f, 0x02, 0x04, 0x02, 0x1f, 0x1f, 0x02, 0x04, 0x1f, 0x0e, 0x11, 0x11, 0x0e, 0x1f, 0x05,
		0x05, 0x02, 0x0e, 0x11, 0x09, 0x16, 0x1f, 0x05, 0x0d, 0x12, 0x12, 0x15, 0x15, 0x09, 0x01, 0x01,
		0x1f, 0x01, 0x01, 0x0f, 0x10, 0x10, 0x0f, 0x03, 0x0c, 0x10, 0x0c, 0x03, 0x07, 0x18, 0x07, 0x18,
		0x07, 0x1b, 0x04, 0x04, 0x1b, 0x17, 0x14, 0x14, 0x0f, 0x19, 0x15, 0x15, 0x13, };

static uint8_t fondIndexTable[] = {
		0x00, 0x04, 0x07, 0x0b, 0x0f, 0x13, 0x17, 0x1b, 0x1f, 0x23, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27,
		0x27, 0x27, 0x2b, 0x2f, 0x33, 0x37, 0x3a, 0x3d, 0x41, 0x45, 0x46, 0x4a, 0x4e, 0x51, 0x56, 0x5a,
		0x5e, 0x62, 0x66, 0x6a, 0x6e, 0x73, 0x77, 0x7c, 0x81, 0x85, 0x89, sizeof(fontDefinition)};

uint8_t sendBuffer[64 * 6 + 1] = { 0x40 };
uint8_t *displayBuffer = sendBuffer + 1;

static void Display_WriteCommand(uint8_t command) {
	HAL_StatusTypeDef status;

	uint8_t buffer[2];
	buffer[0] = 0x00;
	buffer[1] = command;

	status = HAL_I2C_Master_Transmit(&hi2c2, DISPLAY_I2C_ADDRESS << 1, buffer, sizeof(buffer), 100);
	if (status) {
		__BKPT();
	}
}

void Display_SetPixelBuffer(int x, int row, uint8_t value) {
	displayBuffer[row * 64 + x] = value;
}

void Display_Init() {
	// from dsipaly datasheet:
	Display_WriteCommand(0xAE); // Display Off

	Display_WriteCommand(0xD5); //SET DISPLAY CLOCK
	Display_WriteCommand(0x80); //105HZ

	Display_WriteCommand(0xA8); // Select Multiplex Ratio
	Display_WriteCommand(47);

	Display_WriteCommand(0xD3); //Setting Display Offset
	Display_WriteCommand(0x00); //00H Reset, set common start

	Display_WriteCommand(0x40); //Set Display Start Line

	Display_WriteCommand(0x8D); // Set Charge Pump
	Display_WriteCommand(0x14); // Endable Charge Pump

	Display_WriteCommand(0x20);
	Display_WriteCommand(0x00);

	//Set Segment Re-Map Default
	//0xA0 (0x00) => column Address 0 mapped to 127
	//0xA1 (0x01) => Column Address 127 mapped to 0
	Display_WriteCommand(0xA1);

	Display_WriteCommand(0xC8); //Set COM Output Scan Direction

	Display_WriteCommand(0xDA); //Set COM Hardware Configuration
	Display_WriteCommand(0x12);

	Display_WriteCommand(0x81); //Set Contrast Control
	Display_WriteCommand(0xff);

	Display_WriteCommand(0xD9); //Set Pre-Charge period
	Display_WriteCommand(0x22);

	Display_WriteCommand(0xDB); //Set Deselect Vcomh level
	Display_WriteCommand(0x40);

	Display_WriteCommand(0xA4); //Entire Display ON

	Display_WriteCommand(0xA6); //Set Normal Display

	Display_WriteCommand(0xAF); // Display ON

	Display_Clear();
}

void Display_Clear() {
	for (int i = 0; i < 64 * 6; i++) {
		displayBuffer[i] = 0x00;
	}
}

void Display_SendBuffer() {
	HAL_StatusTypeDef status;

	Display_WriteCommand(0x22);
	Display_WriteCommand(0);
	Display_WriteCommand(6);
	Display_WriteCommand(0x21);
	Display_WriteCommand(32);
	Display_WriteCommand(95);

	status = HAL_I2C_Master_Transmit(&hi2c2, DISPLAY_I2C_ADDRESS << 1, sendBuffer, sizeof(sendBuffer), 200);
	if (status) {
		__BKPT();
	}
}

int Display_PrintChar(int x, int row, char ch) {
	if (ch == ':') {
		Display_SetPixelBuffer(x, row, 0x0A);
		Display_SetPixelBuffer(x + 1, row, 0);
		return x + 2;
	}

	if (ch >= 'a' && ch <= 'z') {
		ch -= 32;
	}

	if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z')) {
		ch -= 48;

		size_t startIndex = fondIndexTable[(unsigned char)ch];
		size_t endIndex = fondIndexTable[ch + 1];

		for (size_t i = startIndex; i < endIndex; i++) {
			Display_SetPixelBuffer(x++, row, fontDefinition[i]);
		}
		Display_SetPixelBuffer(x++, row, 0x00);
	}

	return x;
}

int Display_PrintString(int x, int row, char* str) {
	while (*str) {
		x = Display_PrintChar(x, row, *str);
		str++;
	}
}

void Display_ClearLine(int row) {
	for (int i = 0; i < 64; i++) {
		displayBuffer[row * 64 + i] = 0x00;
	}
}
