#ifndef VEML3328_H__
#define VEML3328_H__

#include <stdint.h>

#define VEML3328_I2C_7BIT_ADDRESS			0x10

#define VEML3328_I2C_READ_ADDRESS			((VEML3328_I2C_7BIT_ADDRESS << 1) | 0x01)
#define VEML3328_I2C_WRITE_ADDRESS			((VEML3328_I2C_7BIT_ADDRESS << 1) | 0x00)

#define VEML3328_REG_CONFIG					0x00
#define VEML3328_REG_DATA_C					0x04
#define VEML3328_REG_DATA_R					0x05
#define VEML3328_REG_DATA_G					0x06
#define VEML3328_REG_DATA_B					0x07
#define VEML3328_REG_DATA_IR				0x08
#define VEML3328_REG_DEVICE_ID				0x0C

#define VEML3328_EXPAND_MACRO(content)		content
#define VEML3328_FIELD(NAME)				NAME##_MASK, NAME##_OFFSET
#define VEML3328_GET_FIELD_IMPL(FIELD_MASK,FIELD_OFFSET,value)	(((value) >> FIELD_OFFSET) & FIELD_MASK)
#define VEML3328_SET_FIELD_IMPL(FIELD_MASK,FIELD_OFFSET,value)	(((value) & FIELD_MASK) << FIELD_OFFSET)
#define VEML3328_GET_FIELD(...)				VEML3328_EXPAND_MACRO(VEML3328_GET_FIELD_IMPL(__VA_ARGS__))
#define VEML3328_SET_FIELD(...)				VEML3328_EXPAND_MACRO(VEML3328_SET_FIELD_IMPL(__VA_ARGS__))

#define VEML3328_CONFIG_SD1_MASK			0x1
#define VEML3328_CONFIG_SD1_OFFSET			15
#define VEML3328_CONFIG_SD1_FIELD			VEML3328_FIELD(VEML3328_CONFIG_SD1)

#define VEML3328_CONFIG_SD_ALS_MASK			0x1
#define VEML3328_CONFIG_SD_ALS_OFFSET		14
#define VEML3328_CONFIG_SD_ALS_FIELD		VEML3328_FIELD(VEML3328_CONFIG_SD_ALS)

#define VEML3328_CONFIG_DG_MASK				0x3
#define VEML3328_CONFIG_DG_OFFSET			12
#define VEML3328_CONFIG_DG_FIELD			VEML3328_FIELD(VEML3328_CONFIG_DG)

#define VEML3328_CONFIG_GAIN_MASK			0x3
#define VEML3328_CONFIG_GAIN_OFFSET			10
#define VEML3328_CONFIG_GAIN_FIELD			VEML3328_FIELD(VEML3328_CONFIG_GAIN)

#define VEML3328_CONFIG_SENS_MASK			0x1
#define VEML3328_CONFIG_SENS_OFFSET			6
#define VEML3328_CONFIG_SENS_FIELD			VEML3328_FIELD(VEML3328_CONFIG_SENS)

#define VEML3328_CONFIG_IT_MASK				0x3
#define VEML3328_CONFIG_IT_OFFSET			4
#define VEML3328_CONFIG_IT_FIELD			VEML3328_FIELD(VEML3328_CONFIG_IT)

#define VEML3328_CONFIG_AF_MASK				0x1
#define VEML3328_CONFIG_AF_OFFSET			3
#define VEML3328_CONFIG_AF_FIELD			VEML3328_FIELD(VEML3328_CONFIG_AF)

#define VEML3328_CONFIG_TRIG_MASK			0x1
#define VEML3328_CONFIG_TRIG_OFFSET			2
#define VEML3328_CONFIG_TRIG_FIELD			VEML3328_FIELD(VEML3328_CONFIG_TRIG)

#define VEML3328_CONFIG_SD0_MASK			0x1
#define VEML3328_CONFIG_SD0_OFFSET			0
#define VEML3328_CONFIG_SD0_FIELD			VEML3328_FIELD(VEML3328_CONFIG_SD0)

typedef enum {
	VEML3328_Status_Ok = 0,
	VEML3328_Status_I2CNack = -10,
	VEML3328_Status_I2CError = -11,
	VEML3328_Status_I2CTimeout = -12,
	VEML3328_Status_NotImplemented = -20,
	VEML3328_Status_BadArg = -21,
	VEML3328_Status_InvalidOperation = -22,
	VEML3328_Status_NotSupported = -24,
	VEML3328_Status_UnexpectedDeviceId = -25
} VEML3328_Status;

typedef enum {
	VEML3328_ShutdownMode_PowerOn = 0,
	VEML3328_ShutdownMode_ShutDown = 1
} VEML3328_ShutdownMode;

typedef enum {
	VEML3328_ChannelEnable_All = 0,
	VEML3328_ChannelEnable_GCIR = 1
} VEML3328_ChannelEnable;

typedef enum {
	VEML3328_DigitalGain_1 = 0,
	VEML3328_DigitalGain_2 = 1,
	VEML3328_DigitalGain_4 = 2
} VEML3328_DigitalGain;

typedef enum {
	VEML3328_AnalogGain_1 = 0,
	VEML3328_AnalogGain_2 = 1,
	VEML3328_AnalogGain_4 = 2,
	VEML3328_AnalogGain_1_2 = 3
} VEML3328_AnalogGain;

typedef enum {
	VEML3328_Sensitivity_High = 0,
	VEML3328_Sensitivity_Low = 1
} VEML3328_Sensitivity;

typedef enum {
	VEML3328_IntegrationTime_50ms = 0,
	VEML3328_IntegrationTime_100ms = 1,
	VEML3328_IntegrationTime_200ms = 2,
	VEML3328_IntegrationTime_400ms = 3
} VEML3328_IntegrationTime;

typedef enum {
	VEML3328_TriggerMode_Auto = 0,
	VEML3328_TriggerMode_Force = 1
} VEML3328_TriggerMode;

typedef struct {
	VEML3328_ShutdownMode shutdownMode;
	VEML3328_ChannelEnable channelEnable;
	VEML3328_DigitalGain digitalGain;
	VEML3328_AnalogGain analogGain;
	VEML3328_Sensitivity sensitivity;
	VEML3328_IntegrationTime integrationTime;
	VEML3328_TriggerMode triggerMode;
} VEML3328_Configuration;

VEML3328_Status VEML3328_Init();
VEML3328_Status VEML3328_Deinit();

VEML3328_Status VEML3328_GetDefaultConfiguration(VEML3328_Configuration* config);
VEML3328_Status VEML3328_GetConfiguration(VEML3328_Configuration* config);
VEML3328_Status VEML3328_SetConfiguration(VEML3328_Configuration* config);

VEML3328_Status VEML3328_TriggerSingleShoot();

VEML3328_Status VEML3328_ReadR(uint16_t* r);
VEML3328_Status VEML3328_ReadG(uint16_t* g);
VEML3328_Status VEML3328_ReadB(uint16_t* b);
VEML3328_Status VEML3328_ReadC(uint16_t* c);
VEML3328_Status VEML3328_ReadIR(uint16_t* ir);
VEML3328_Status VEML3328_ReadRGB(uint16_t* r, uint16_t* g, uint16_t* b);
VEML3328_Status VEML3328_ReadGCIR(uint16_t* g, uint16_t* c, uint16_t* ir);
VEML3328_Status VEML3328_ReadRGBCIR(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c, uint16_t* ir);

VEML3328_Status VEML3328_GetDeviceId(uint8_t* deviceId);

#endif
