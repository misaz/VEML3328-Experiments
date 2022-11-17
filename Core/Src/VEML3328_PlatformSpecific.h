#ifndef VEML3328_PLATFORM_SPECIFIC_H__
#define VEML3328_PLATFORM_SPECIFIC_H__

#include "VEML3328.h"

#include <stdint.h>

VEML3328_Status VEML3328_PlatformSpecific_Init();
VEML3328_Status VEML3328_PlatformSpecific_Deinit();
VEML3328_Status VEML3328_PlatformSpecific_Read(uint8_t registerAddress, uint16_t* value);
VEML3328_Status VEML3328_PlatformSpecific_Write(uint8_t registerAddress, uint16_t value);

#endif