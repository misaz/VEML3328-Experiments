/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "VEML3328.h"
#include "Display.h"
#include "WS2815B.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void DrawProgressBar(int row, int percent) {
	percent /= 2;

	Display_SetPixelBuffer(11, row, 0x3F);
	int i = 0;
	for (i = 0; i < percent && i < 50; i++) {
		Display_SetPixelBuffer(i + 12, row, 0x3F);
	}
	for (; i < 50; i++) {
		Display_SetPixelBuffer(i + 12, row, 0x21);
	}
	Display_SetPixelBuffer(62, row, 0x3F);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	VEML3328_Status vemlStatus;
	VEML3328_Configuration vemlConfig;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

	GPIO_InitTypeDef sclClockOutGpioMode;
	sclClockOutGpioMode.Mode = GPIO_MODE_OUTPUT_OD;
	sclClockOutGpioMode.Pin = GPIO_PIN_11;
	sclClockOutGpioMode.Alternate = GPIO_AF6_I2C2;
	sclClockOutGpioMode.Pull = GPIO_PULLUP;
	sclClockOutGpioMode.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &sclClockOutGpioMode);

	for (int i = 0; i < 20; i++) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);
		HAL_Delay(10);
	}

	sclClockOutGpioMode.Mode = GPIO_MODE_AF_OD;
	HAL_GPIO_Init(GPIOA, &sclClockOutGpioMode);

	vemlStatus = VEML3328_Init();
	if (vemlStatus) {
		HAL_Delay(10);
		HAL_NVIC_SystemReset();
	}

	Display_Init();

	// print R G B C IR letter
	Display_PrintString(0, 0, "R:");
	Display_PrintString(0, 1, "G:");
	Display_PrintString(0, 2, "B:");
	Display_PrintString(0, 3, "C:");
	Display_PrintString(0, 4, "IR:");
	Display_SendBuffer();

	VEML3328_GetDefaultConfiguration(&vemlConfig);
	vemlConfig.shutdownMode = VEML3328_ShutdownMode_PowerOn;
	//vemlConfig.digitalGain = VEML3328_DigitalGain_4;
	vemlConfig.analogGain = VEML3328_AnalogGain_2;
	//vemlConfig.channelEnable = VEML3328_ChannelEnable_GCIR;
	vemlConfig.integrationTime = VEML3328_IntegrationTime_100ms;
	vemlStatus = VEML3328_SetConfiguration(&vemlConfig);
	if (vemlStatus) {
		__BKPT(0);
	}

	WS2815B_Init();
	uint8_t whiteLevel = 0;
	for (int i = 0; i < WS2815B_MAX_LEDS; i++) {
		WS2815B_SetColor(i, whiteLevel, whiteLevel, whiteLevel);
	}
	WS2815B_SendsData();
	HAL_Delay(10);
	WS2815B_SendsData();

	uint16_t lastR, lastG, lastB, lastC, lastIr;

	float whiteBalancingRCoef = 1;
	float whiteBalancingGCoef = 1;
	float whiteBalancingBCoef = 1;

	uint16_t minimumC = 65535;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		uint16_t r, g, b, c, ir;

		vemlStatus = VEML3328_ReadRGBCIR(&r, &g, &b, &c, &ir);
		if (vemlStatus) {
			__BKPT(0);
		}

		if (r != lastR || g != lastG || b != lastB || c != lastC || ir != lastIr) {
			lastR = r;
			lastG = g;
			lastB = b;
			lastC = c;
			lastIr = ir;

			/*
			if (c < minimumC && c > 100) {
				minimumC = c;
			}
			*/
			if (c > 100 && minimumC == 65535) {
				minimumC = c;
			}

			float balancedR = whiteBalancingRCoef * (float) r;
			float balancedG = whiteBalancingGCoef * (float) g;
			float balancedB = whiteBalancingBCoef * (float) b;

			float fullScale = 5000;
			DrawProgressBar(0, (int) (100 * balancedR / fullScale));
			DrawProgressBar(1, (int) (100 * balancedG / fullScale));
			DrawProgressBar(2, (int) (100 * balancedB / fullScale));
			DrawProgressBar(3, (int) (100 * (float) c / fullScale));
			DrawProgressBar(4, (int) (100 * (float) ir / fullScale));
			Display_ClearLine(5);

			if (c > (float)minimumC * 1.5) {
				float maxVal = balancedR;
				if (balancedG > maxVal) {
					maxVal = balancedG;
				}
				if (balancedB > maxVal) {
					maxVal = balancedB;
				}

				uint8_t color = 0;
				if (balancedR / (float)maxVal > 0.8) {
					color |= 0b001;
				}
				if (balancedG / (float)maxVal > 0.8) {
					color |= 0b010;
				}
				if (balancedB / (float)maxVal > 0.8) {
					color |= 0b100;
				}

				switch (color) {
				case 0b001:
					Display_PrintString(0, 5, "COLOR: RED");
					break;
				case 0b010:
					Display_PrintString(0, 5, "COLOR: GREEN");
					break;
				case 0b100:
					Display_PrintString(0, 5, "COLOR: BLUE");
					break;
				case 0b011:
					Display_PrintString(0, 5, "COLOR: YELLOW");
					break;
				case 0b110:
					Display_PrintString(0, 5, "COLOR: CYAN");
					break;
				case 0b101:
					Display_PrintString(0, 5, "COLOR: MAGENTA");
					break;
				case 0b111:
					Display_PrintString(0, 5, "COLOR: WHITE");
					break;
				}
			}

			Display_SendBuffer();

			WS2815B_SendsData();
		}

		GPIO_InitTypeDef btn;
		btn.Mode = GPIO_MODE_INPUT;
		btn.Alternate = 0;
		btn.Pin = GPIO_PIN_1;
		btn.Pull = GPIO_NOPULL;
		btn.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &btn);

		// white ballancing
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0) {
			uint16_t max = lastR;
			if (lastG > max) {
				max = lastG;
			}
			if (lastB > max) {
				max = lastB;
			}

			whiteBalancingRCoef = max / (float)lastR;
			whiteBalancingGCoef = max / (float)lastG;
			whiteBalancingBCoef = max / (float)lastB;
		}

		HAL_Delay(1);

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00101319;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
