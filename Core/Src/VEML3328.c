#include "VEML3328.h"
#include "VEML3328_PlatformSpecific.h"

#include <stdint.h>
#include <stddef.h>

static VEML3328_Configuration VEML3328_CurrentConfig;

VEML3328_Status VEML3328_Init() {
	VEML3328_Status status;
	uint8_t deviceId;

	status = VEML3328_PlatformSpecific_Init();
	if (status) {
		return status;
	}

	status = VEML3328_GetDeviceId(&deviceId);
	if (status) {
		return status;
	}

	if (deviceId != 0x28) {
		return VEML3328_Status_UnexpectedDeviceId;
	}

	return VEML3328_GetConfiguration(&VEML3328_CurrentConfig);
}

VEML3328_Status VEML3328_Deinit() {
	VEML3328_CurrentConfig.shutdownMode = VEML3328_ShutdownMode_ShutDown;

	// do not worry about error code in deinit
	VEML3328_SetConfiguration(&VEML3328_CurrentConfig);

	return VEML3328_PlatformSpecific_Deinit();
}

VEML3328_Status VEML3328_GetDefaultConfiguration(VEML3328_Configuration* config) {
	config->shutdownMode = VEML3328_ShutdownMode_ShutDown;
	config->channelEnable = VEML3328_ChannelEnable_All;
	config->digitalGain = VEML3328_DigitalGain_1;
	config->analogGain = VEML3328_AnalogGain_1;
	config->sensitivity = VEML3328_Sensitivity_High;
	config->integrationTime = VEML3328_IntegrationTime_50ms;
	config->triggerMode = VEML3328_TriggerMode_Auto;

	return VEML3328_Status_Ok;
}

VEML3328_Status VEML3328_GetConfiguration(VEML3328_Configuration* config) {
	VEML3328_Status status;

	uint16_t configReg;

	status = VEML3328_PlatformSpecific_Read(VEML3328_REG_CONFIG, &configReg);
	if (status) {
		return status;
	}

	config->shutdownMode = VEML3328_GET_FIELD(VEML3328_CONFIG_SD1_FIELD, configReg);
	config->channelEnable = VEML3328_GET_FIELD(VEML3328_CONFIG_SD_ALS_FIELD, configReg);
	config->digitalGain = VEML3328_GET_FIELD(VEML3328_CONFIG_DG_FIELD, configReg);
	config->analogGain = VEML3328_GET_FIELD(VEML3328_CONFIG_GAIN_FIELD, configReg);
	config->sensitivity = VEML3328_GET_FIELD(VEML3328_CONFIG_SENS_FIELD, configReg);
	config->integrationTime = VEML3328_GET_FIELD(VEML3328_CONFIG_IT_FIELD, configReg);
	config->triggerMode = VEML3328_GET_FIELD(VEML3328_CONFIG_AF_FIELD, configReg);

	return VEML3328_Status_Ok;
}

VEML3328_Status VEML3328_SetConfiguration(VEML3328_Configuration* config) {
	VEML3328_Status status;

	uint16_t configReg =
		VEML3328_SET_FIELD(VEML3328_CONFIG_SD1_FIELD, config->shutdownMode) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_SD_ALS_FIELD, config->channelEnable) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_DG_FIELD, config->digitalGain) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_GAIN_FIELD, config->analogGain) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_SENS_FIELD, config->sensitivity) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_IT_FIELD, config->integrationTime) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_AF_FIELD, config->triggerMode);

	status = VEML3328_PlatformSpecific_Write(VEML3328_REG_CONFIG, configReg);
	if (status) {
		return status;
	}

	VEML3328_CurrentConfig.shutdownMode = config->shutdownMode;
	VEML3328_CurrentConfig.channelEnable = config->channelEnable;
	VEML3328_CurrentConfig.digitalGain = config->digitalGain;
	VEML3328_CurrentConfig.analogGain = config->analogGain;
	VEML3328_CurrentConfig.sensitivity = config->sensitivity;
	VEML3328_CurrentConfig.integrationTime = config->integrationTime;
	VEML3328_CurrentConfig.triggerMode = config->triggerMode;

	return VEML3328_Status_Ok;
}

VEML3328_Status VEML3328_TriggerSingleShoot() {
	if (VEML3328_CurrentConfig.shutdownMode == VEML3328_ShutdownMode_ShutDown) {
		return VEML3328_Status_InvalidOperation;
	}

	uint16_t configReg =
		VEML3328_SET_FIELD(VEML3328_CONFIG_SD1_FIELD, VEML3328_CurrentConfig.shutdownMode) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_SD_ALS_FIELD, VEML3328_CurrentConfig.channelEnable) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_DG_FIELD, VEML3328_CurrentConfig.digitalGain) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_GAIN_FIELD, VEML3328_CurrentConfig.analogGain) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_SENS_FIELD, VEML3328_CurrentConfig.sensitivity) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_IT_FIELD, VEML3328_CurrentConfig.integrationTime) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_AF_FIELD, VEML3328_CurrentConfig.triggerMode) |
		VEML3328_SET_FIELD(VEML3328_CONFIG_TRIG_FIELD, 1);

	return VEML3328_PlatformSpecific_Write(VEML3328_REG_CONFIG, configReg);
}

VEML3328_Status VEML3328_ReadR(uint16_t* r) {
	return VEML3328_PlatformSpecific_Read(VEML3328_REG_DATA_R, r);
}

VEML3328_Status VEML3328_ReadG(uint16_t* g) {
	return VEML3328_PlatformSpecific_Read(VEML3328_REG_DATA_G, g);
}

VEML3328_Status VEML3328_ReadB(uint16_t* b) {
	return VEML3328_PlatformSpecific_Read(VEML3328_REG_DATA_B, b);
}

VEML3328_Status VEML3328_ReadC(uint16_t* c) {
	return VEML3328_PlatformSpecific_Read(VEML3328_REG_DATA_C, c);
}

VEML3328_Status VEML3328_ReadIR(uint16_t* ir) {
	return VEML3328_PlatformSpecific_Read(VEML3328_REG_DATA_IR, ir);
}

VEML3328_Status VEML3328_ReadRGB(uint16_t* r, uint16_t* g, uint16_t* b) {
	VEML3328_Status status;

	status = VEML3328_ReadR(r);
	if (status) {
		return status;
	}

	status = VEML3328_ReadG(g);
	if (status) {
		return status;
	}

	status = VEML3328_ReadB(b);
	if (status) {
		return status;
	}

	return VEML3328_Status_Ok;
}

VEML3328_Status VEML3328_ReadGCIR(uint16_t* g, uint16_t* c, uint16_t* ir) {
	VEML3328_Status status;

	status = VEML3328_ReadG(g);
	if (status) {
		return status;
	}

	status = VEML3328_ReadC(c);
	if (status) {
		return status;
	}

	status = VEML3328_ReadIR(ir);
	if (status) {
		return status;
	}

	return VEML3328_Status_Ok;
}

VEML3328_Status VEML3328_ReadRGBCIR(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c, uint16_t* ir) {
	VEML3328_Status status;

	status = VEML3328_ReadR(r);
	if (status) {
		return status;
	}

	status = VEML3328_ReadG(g);
	if (status) {
		return status;
	}

	status = VEML3328_ReadB(b);
	if (status) {
		return status;
	}

	status = VEML3328_ReadC(c);
	if (status) {
		return status;
	}

	status = VEML3328_ReadIR(ir);
	if (status) {
		return status;
	}

	return VEML3328_Status_Ok;
}

VEML3328_Status VEML3328_GetDeviceId(uint8_t* deviceId) {
	VEML3328_Status status;
	uint16_t regVal;

	status = VEML3328_PlatformSpecific_Read(VEML3328_REG_DEVICE_ID, &regVal);
	if (status) {
		return status;
	}

	*deviceId = (uint8_t)(regVal & 0xFF);

	return VEML3328_Status_Ok;
}
